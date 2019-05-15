#version 450

layout(binding = 2) uniform sampler2D positions;
layout(binding = 3) uniform sampler2D normals;
layout(binding = 4) uniform sampler2D colors;

layout (location = 4) uniform vec3 camera_position;
layout(location = 13) uniform ivec2 screenResolution;

in vec3 light_center;
in vec3 light_color;
in float light_size;

layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 fragColor;


void main(){
	float screenGamma = 2.2;
	vec2 uv = gl_FragCoord.xy/screenResolution;
	vec3 normal = texture(normals, uv).xyz;
	vec3 diff_color = texture(colors, uv).xyz;
	vec3 position = texture(positions, uv).xyz;
	
	float light_power = light_size*50;
	
	vec3 lightDir = light_center - position;
	float distance = length(lightDir);

	float distance_sq = distance*distance;
	lightDir = normalize(lightDir);
	
	float lambertian = max(dot(lightDir,normal), 0.0);
	float specular = 0.0;
	
	if (lambertian > 0.0) {
		vec3 viewDir = normalize(camera_position - position);
		
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0);
		specular = pow(specAngle, 16); //shinnines
	}
	
	if (distance <= light_size*1.2)
			specular = 1;
	
	
	vec3 colorLinear = diff_color * lambertian *light_power/distance_sq + specular*light_color*light_power/distance_sq;
	
	vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
	
	fragColor = vec4(colorGammaCorrected,1.0);
}