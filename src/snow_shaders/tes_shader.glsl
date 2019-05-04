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

out vec3 diffuse_color;
out vec3 world_position;

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
	
	if (insideBox(camera_delta, -world_texture_size/2, world_texture_size/2)==1){
		vec2 point = wrap(point, world_texture_size);
		ivec2 tex_coords = ivec2(floor( point*pixel_resolution ));
		diffuse_color = vec3(1);
		return tex_coords;
	} else {
		return ivec2(-1,-1);
	}
}



float getDeformedHeight(vec2 world_pos){
	float ground_height = getGroundHeight(world_pos);
	float height = ground_height+snow_height;
	
	vec2 world_texture_size = imageSize(deformation_texture)/pixel_resolution;
	ivec2 tex_coords = pointToTextureUV(world_pos, world_texture_size);
	
	float deformation_height = height;
	if (tex_coords.x > -1){
		uint res = uint(imageLoad(deformation_texture,tex_coords));
		uint def = res >> 16;
		//uint height = unt(round(deform_point_height));
		deformation_height = min(height, def);
		deformation_height = max(ground_height+0.03, deformation_height);
	}

	return deformation_height;
}

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]);
 	diffuse_color = vec3(1,1,0) ;//interpolate3D(color_es[0], color_es[1], color_es[2]);
 	
 	float height = getDeformedHeight(world_pos.xz);
 	
	vec3 pos = vec3(world_pos.x, height ,world_pos.z);
	world_position = pos;
	
	gl_Position = mvp*vec4(pos, 1.0);
}