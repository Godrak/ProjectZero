#version 450 core

layout (local_size_x = 32, local_size_y = 32) in;

layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 4) uniform vec3 camera_position;
layout(location = 6) uniform float time_delta;
layout(location = 8) uniform float pixel_resolution;
layout(location = 11) uniform float snow_fill_rate;


vec2 wrap (vec2 world_pos, vec2 texture_size){
	return world_pos-(texture_size*floor ( world_pos / texture_size));
}

ivec2 pointToTextureUV(vec2 point, vec2 world_texture_size){
	point = wrap(point, world_texture_size);
	ivec2 tex_coords = ivec2(floor( point*pixel_resolution ));
	return tex_coords;
}


void main(){
	uvec2 texture_size = imageSize(deformation_texture);
	vec2 world_texture_size = imageSize(deformation_texture)/pixel_resolution;
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

	ivec2 halfTexSize = ivec2(texture_size/2);
	ivec2 center = pointToTextureUV(camera_position.xz, world_texture_size);
	ivec2 lcoords = coords + halfTexSize - center;
	if (lcoords.x < -halfTexSize.x)
		lcoords.x+=int(texture_size.x);
	if (lcoords.y < -halfTexSize.y)
		lcoords.y+=int(texture_size.y);
	if (lcoords.x > halfTexSize.x)
		lcoords.x-=int(texture_size.x);
	if (lcoords.y > halfTexSize.y)
		lcoords.y-=int(texture_size.y);
	
	float distance = length(halfTexSize-abs(lcoords))/(length(texture_size/2));
	
	float ratio = pow(distance,7);
	
	uint res = uint(imageLoad(deformation_texture,coords));
	uint def_height = res >> 16;
	uint height = (res << 16) >> 16;

	float ffill = ratio*time_delta*snow_fill_rate;
	uint fill = uint(round(ffill));
	
		
	uint new_def_height = def_height +fill;
	uint new_height = height + fill;
	
	
	uint deformation_height = min(new_def_height, 60000);
	uint deform_point_height = min(new_height, 60000);
	
	uint def = deformation_height << 16;
	res = def | deform_point_height;
	imageAtomicExchange(deformation_texture,coords,res);
}






