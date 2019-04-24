#version 450

in vec3 diffuse_color;
in vec3 model_position;
in vec3	world_position;


out vec4 fragColor;

void main(){
	fragColor = vec4(diffuse_color, 1.0);
}