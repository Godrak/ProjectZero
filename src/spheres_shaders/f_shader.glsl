#version 450

in vec3 color;
in vec3 normal;

out vec4 fragColor;

void main(){
	fragColor = vec4(color+normal*0.2+vec3(0.1,0.1,0.1), 1.0);
}