#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

layout(binding = 0) uniform sampler2D heightmap;

layout(location = 0) uniform mat4 mvp;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;

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

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]); 	
 	float height = getGroundHeight(world_pos.xz);
	vec3 pos = vec3(world_pos.x, height ,world_pos.z);
	
 	diffuse_color = color_es[0];
	world_position = pos;
	
	gl_Position = mvp*vec4(pos, 1.0);
}

