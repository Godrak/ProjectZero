#version 450

/*
layout(location = 0) uniform mat4 mvp;
layout(location = 1) uniform sampler2D heightmap;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 color;

void main(){
	color = vertex_color;
	float s = vertex_position.x / terrain_size.x;
	float t = vertex_position.z / terrain_size.y;
	vec4 tex_val = texture(heightmap, vec2(s,t));
	vec3 pos = vec3(vertex_position.x, tex_val.x*vertical_scaling ,vertex_position.z);
	gl_Position = mvp*vec4(pos, 1.0);
}*/


layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 color;
out vec3 position;

void main(){
	color = vertex_color;
	position = vertex_position;
}