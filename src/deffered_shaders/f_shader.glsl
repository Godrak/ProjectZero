#version 450

layout(binding = 2) uniform sampler2D positions;
layout(binding = 3) uniform sampler2D normals;
layout(binding = 4) uniform sampler2D colors;

layout(location = 13) uniform ivec2 screenResolution;

in vec4 gl_FragCoord;
out vec4 fragColor;


void main(){
	vec2 uv = gl_FragCoord.xy/screenResolution;
	vec3 normal = texture(normals, uv).xyz;
	vec3 diff_color = texture(colors, uv).xyz;
	vec3 position = texture(positions, uv).xyz;
		
	fragColor = vec4(normal,1.0);
}