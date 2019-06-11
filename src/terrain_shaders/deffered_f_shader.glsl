#version 450

layout(binding = 0) uniform sampler2D heightmap;
layout(binding = 6) uniform sampler2D snow_normals_texture;
layout(binding = 8) uniform sampler2D snow_color_texture;
layout(binding = 9) uniform sampler2D snow_ambient_texture;

layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 10) uniform float normal_offset;
layout(location = 16) uniform float snow_texture_size;


in vec3	world_position;
in vec3 diffuse_color;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;


vec3 getSnowTextureColor(vec2 location){
	return texture(snow_color_texture, location/snow_texture_size).xyz;
}

vec3 getSnowTextureNormal(vec2 location){
	return normalize(2*texture(snow_normals_texture, location/snow_texture_size).xzy - 1);
}

float getSnowTextureAmbient(vec2 location){
	return texture(snow_ambient_texture, location/snow_texture_size).x;
}



float getGroundHeight(vec2 world_pos){
	vec2 uv = world_pos / terrain_size;
	vec4 height = texture(heightmap, uv);
	return height.x*vertical_scaling;
}

vec2 wrap (vec2 world_pos, vec2 texture_size){
	return world_pos-(texture_size*floor ( world_pos / texture_size));
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
	vec3 dnormal = getGroundNormal(world_position.xz);
	
	float ambient = getSnowTextureAmbient(world_position.xz);
	vec3 tcolor = sqrt(getSnowTextureColor(world_position.xz));
	vec3 tnormal = getSnowTextureNormal(world_position.xz);
	
	normal = normalize((dnormal + tnormal));
	color = ambient*tcolor;
}