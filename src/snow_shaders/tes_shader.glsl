#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

layout(binding = 0) uniform sampler2D heightmap;
layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 0) uniform mat4 mvp;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 8) uniform float pixel_resolution;
layout(location = 9) uniform float snow_height;


in vec3[] position_es;
in vec3[] color_es;

out vec3 color;

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float getGroundHeight(float world_x, float world_z){
	float s = world_x / terrain_size.x;
	float t = world_z / terrain_size.y;
	vec4 height = texture(heightmap, vec2(s,t));
	return height.x*vertical_scaling;
}

// return 1 if v inside the box, return 0 otherwise
float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;   
}

float getDeformedHeight(vec3 world_pos){
	float height = getGroundHeight(world_pos.x, world_pos.z)+snow_height;
	
	ivec2 texture_size = imageSize(deformation_texture);
	vec2 camera_delta = world_pos.xz - camera_position.xz;
	vec2 pixel_delta = camera_delta*pixel_resolution;
	ivec2 tex_coords = ivec2(round( pixel_delta+texture_size/2 ));
	
	float deformation_height = height;
	if (insideBox(tex_coords, vec2(0,0), texture_size)==1){
		uint res = uint(imageLoad(deformation_texture,tex_coords));
		uint def = res >> 16;
		//uint height = unt(round(deform_point_height));
		deformation_height = min(height, def);
		color = vec3(1);
	}

	return deformation_height;
}

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]);
 	color = vec3(1,1,0) ;//interpolate3D(color_es[0], color_es[1], color_es[2]);
 	
 	float height = getDeformedHeight(world_pos);
 	
	vec3 pos = vec3(world_pos.x, height ,world_pos.z);
	gl_Position = mvp*vec4(pos, 1.0);
}