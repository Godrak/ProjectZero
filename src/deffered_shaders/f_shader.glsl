#version 450

layout(binding = 2) uniform sampler2D positions;
layout(binding = 3) uniform sampler2D normals;
layout(binding = 4) uniform sampler2D colors;

in vec2 uv;
out vec4 fragColor;

void main(){
	vec4 color = texture(normals, uv);
	
	fragColor = color;
}