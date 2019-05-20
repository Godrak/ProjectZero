#version 450

in vec3 diffuse_color;
in vec3 model_position;
in vec3	world_position;

out vec4 fragColor;
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;

void main(){
	position = world_position;
	normal = vec3(0,0,0);//normalize(normalize(model_position.xzy)/2.0+vec3(1));
	color = diffuse_color;
}