#version 450

layout(binding = 5) uniform sampler2D depth_buffer;
layout(binding = 7) uniform samplerCube skybox_texture;

layout(location = 13) uniform ivec2 screenResolution;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out vec4 color;

in vec3 direction;

void main(){
	vec2 uv = gl_FragCoord.xy/screenResolution;
	float depth = texture(depth_buffer, uv).x;
	vec3 skyColor = texture(skybox_texture, direction).xyz;
	
	if (depth < 0.99999){
		discard;
	}

	//color = vec4(depth,depth,depth,1.0);
	color = vec4(skyColor, 1.0);
}