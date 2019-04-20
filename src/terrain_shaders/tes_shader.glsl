#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

layout(binding = 0) uniform sampler2D heightmap;

layout(location = 0) uniform mat4 mvp;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;

in vec3[] position_es;
in vec3[] color_es;


out vec3 diffuse_color;
out vec3 world_normal;
out vec3 world_position;

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

vec3 getGroundNormal(vec2 world_pos){
	float scale = 1;
	vec2 right = world_pos+vec2(scale,0);
	vec2 left = world_pos-vec2(scale,0);
	vec2 down = world_pos-vec2(scale,0);
	vec2 up = world_pos+vec2(scale,0);
	
	float r_height = getGroundHeight(right.x, right.y);
	float l_height = getGroundHeight(left.x, left.y);
	float u_height = getGroundHeight(up.x, up.y);
	float d_height = getGroundHeight(down.x, down.y);
	
	vec3 tangent = vec3(2*scale, r_height - l_height, 0.0);
	vec3 bitangent = vec3(0.0, d_height - u_height, 2*scale);
	vec3 normal = normalize(cross(tangent, bitangent));

	return normal;
}

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]); 	
 	float height = getGroundHeight(world_pos.x, world_pos.z);
	vec3 pos = vec3(world_pos.x, height ,world_pos.z);
	
 	diffuse_color = color_es[0];
	world_position = pos;
	world_normal = getGroundNormal(world_pos.xz);
	
	gl_Position = mvp*vec4(pos, 1.0);
}

