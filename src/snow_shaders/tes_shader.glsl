#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

layout(binding = 0) uniform sampler2D heightmap;
layout(r32ui, binding = 1) uniform uimage2D deformation_texture;
layout(binding = 10) uniform sampler2D snow_height_displacement;

layout(location = 0) uniform mat4 mvp;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 8) uniform float pixel_resolution;
layout(location = 9) uniform float snow_height;
layout(location = 16) uniform float snow_texture_size;
layout(location = 17) uniform float snow_height_displacement_size;

in vec3[] position_es;
in vec3[] color_es;

out vec3 diffuse_color;
out vec3 world_position;

float getSnowTextureHeight(vec2 location){
	return texture(snow_height_displacement, location/snow_texture_size).x * snow_height_displacement_size;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float getGroundHeight(vec2 world_pos){
	vec2 uv = world_pos / terrain_size;
	vec4 height = texture(heightmap, uv);
	return height.x*vertical_scaling;
}

float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;   
}

vec2 wrap (vec2 world_pos, vec2 texture_size){
	return world_pos-(texture_size*floor ( world_pos / texture_size));
}

ivec2 pointToTextureUV(vec2 point, vec2 world_texture_size){
	vec2 camera_delta = point - camera_position.xz;
	
	if (insideBox(camera_delta, -world_texture_size/2, world_texture_size/2) > 0.5){
		vec2 point = wrap(point, world_texture_size);
		ivec2 tex_coords = ivec2(floor( point*pixel_resolution ));
		diffuse_color = vec3(0.4,0.8,1);
		return tex_coords;
	} else {
		return ivec2(-1,-1);
	}
}

float computeDepressionOrElevation(float deformation_height, float deform_point_height, float total_height){
	if (deform_point_height > total_height || 
		total_height - deformation_height > 3*snow_height || 
		total_height - deform_point_height > 3*snow_height ||
		total_height - deform_point_height <= 0)
		return total_height;
	
	if (deformation_height < total_height)
		return deformation_height;

	float depression_distance = sqrt(max(total_height - deform_point_height,0));
	float distance_from_foot = sqrt(max(deformation_height - deform_point_height,0));
	
	float depression_depth = abs(total_height - deform_point_height);
	 
	float elevation_distance = distance_from_foot - depression_distance;
	float maximum_elevation_distance  = snow_height*0.1;	
	float ratio = abs(elevation_distance / maximum_elevation_distance);
	
	float height = maximum_elevation_distance*5.0;
	float ratio6 = ratio*ratio*ratio*ratio*ratio*ratio;
	float t = 2*sqrt((ratio)/(1+10*ratio6)) / (1+10*ratio*ratio);
	
	float elevation = t*height;
	return total_height + elevation;
}


float getDeformedHeight(vec2 world_pos, float height){
	
	vec2 world_texture_size = imageSize(deformation_texture)/pixel_resolution;
	ivec2 tex_coords = pointToTextureUV(world_pos, world_texture_size);
	
	float deformation_height = height;
	if (tex_coords.x > -1){
	
		uint res = uint(imageLoad(deformation_texture,tex_coords));
		uint deform_height = res >> 16;
		uint deform_point_height = (res << 16) >> 16;
		
		deformation_height = computeDepressionOrElevation(deform_height, deform_point_height, height);
	}

	return deformation_height;
}

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]);
 	diffuse_color = vec3(1,1,0) ;//interpolate3D(color_es[0], color_es[1], color_es[2]);
 	
 	float height = getGroundHeight(world_pos.xz)+snow_height;
 	height = getDeformedHeight(world_pos.xz, height);
 	height += getSnowTextureHeight(world_pos.xz);
 	
	vec3 pos = vec3(world_pos.x, height ,world_pos.z);
	world_position = pos;
	
	gl_Position = mvp*vec4(pos, 1.0);
}