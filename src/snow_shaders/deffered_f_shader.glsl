#version 450

layout(binding = 0) uniform sampler2D heightmap;
layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 8) uniform float pixel_resolution;
layout(location = 9) uniform float snow_height;
layout(location = 10) uniform float normal_off;

in vec3	world_position;
in vec3 diffuse_color;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;

float getGroundHeight(vec2 world_pos){
	vec2 uv = world_pos / terrain_size;
	vec4 height = texture(heightmap, uv);
	return height.x*vertical_scaling;
}

// return 1 if v inside the box, return 0 otherwise
float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;   
}

vec2 wrap (vec2 world_pos, vec2 texture_size){
	return world_pos-(texture_size*floor ( world_pos / texture_size));
}

ivec2 pointToTextureUV(vec2 point, vec2 world_texture_size){
	vec2 camera_delta = point - camera_position.xz;
	
	if (insideBox(camera_delta, -world_texture_size/2 + 1, world_texture_size/2 -1)==1){
		vec2 point = wrap(point, world_texture_size);
		ivec2 tex_coords = ivec2(floor( point*pixel_resolution ));
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


vec3 getGroundNormal(vec2 world_pos){
	float normal_offset = normal_off;
	vec2 right = world_pos+vec2(normal_offset,0);
	vec2 left = world_pos-vec2(normal_offset,0);
	vec2 down = world_pos-vec2(normal_offset,0);
	vec2 up = world_pos+vec2(normal_offset,0);
	
	float r_height = getDeformedHeight(right);
	float l_height = getDeformedHeight(left);
	float u_height = getDeformedHeight(up);
	float d_height = getDeformedHeight(down);
	
	vec3 tangent = vec3(2*normal_offset, r_height - l_height, 0.0);
	vec3 bitangent = vec3(0.0, d_height - u_height, 2*normal_offset);
	vec3 normal = normalize(-cross(tangent, bitangent));

	return normal;
}

void main(){
	position = world_position;
	normal = getGroundNormal(world_position.xz);
	color = diffuse_color;
}