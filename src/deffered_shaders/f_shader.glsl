#version 450

in vec3 diffuse_color;
in vec3 world_normal;
in vec3	world_position;

out vec4 fragColor;
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;

void main(){
	position = world_position;
	normal = world_normal;
	color = diffuse_color;
}