#version 450

layout(binding = 0) uniform sampler2D heightmap;

layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 10) uniform float normal_offset;

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

vec3 getGroundNormal(vec2 world_pos){
	vec2 off = vec2(normal_offset, 0.0);
	
	float r_height = getGroundHeight(world_pos + off.xy);
	float l_height = getGroundHeight(world_pos - off.xy);
	float u_height = getGroundHeight(world_pos + off.yx);
	float d_height = getGroundHeight(world_pos - off.yx);
	
	vec3 tangent = vec3(2*off.x, r_height - l_height, 0.0);
	vec3 bitangent = vec3(0.0, u_height - d_height, 2*off.x);
	vec3 normal = cross(bitangent, tangent);

	return normalize(normal);
}

void main(){
	position = world_position;
	normal = getGroundNormal(world_position.xz);
	color = diffuse_color;
}