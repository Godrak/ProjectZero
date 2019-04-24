#version 450 core

layout (local_size_x = 32, local_size_y = 32) in;

layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 6) uniform float time_delta;
layout(location = 11) uniform float snow_fill_rate;


void main(){
	uvec2 texture_size = imageSize(deformation_texture);
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	float distance = length(coords-texture_size/2.0);
	float max_distance = length(texture_size/2.0);
	
	uint res = uint(imageLoad(deformation_texture,coords));
	uint def_height = res >> 16;
	uint height = (res << 16) >> 16;
	
	float ratio = distance/max_distance;
	uint new_def_height = def_height+ uint(round(1*time_delta*snow_fill_rate));
	uint new_height = height + uint(round(1*time_delta*snow_fill_rate));
	
	
	uint deformation_height = min(new_def_height,60000);
	uint deform_point_height = min(new_height, 60000);
	
	uint def = deformation_height << 16;
	res = def | deform_point_height;
	imageAtomicExchange(deformation_texture,coords,res);
}






