#version 450

layout(binding = 2) uniform sampler2D positions;
layout(binding = 3) uniform sampler2D normals;
layout(binding = 4) uniform sampler2D colors;

layout (location = 4) uniform vec3 cameraPosition;
layout(location = 13) uniform ivec2 screenResolution;
layout(location = 14) uniform vec3 lightParams; //Kc, Kl, Kq

in vec3 light_center;
in vec3 light_color;
in float light_size;
in flat int ambientFlag;


layout(pixel_center_integer) in vec4 gl_FragCoord;
out vec4 fragColor;


void main(){
	float screenGamma = 2.2;

	vec2 uv = gl_FragCoord.xy/screenResolution;
	vec3 normal = texture(normals, uv).xyz;
	vec3 diff_color = texture(colors, uv).xyz;
	vec3 position = texture(positions, uv).xyz;
	
	vec3 lightDir = (light_center - position);
	float distance = length(lightDir);
	float distance_sq = distance*distance;
	lightDir = normalize(lightDir);
	
	vec3 viewDir = normalize(cameraPosition - position);
	
	// diffuse
  	vec3 diffuse = max(dot(normal, lightDir), 0.0) * diff_color;
  	
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    if (normal == vec3(0,0,0) && ambientFlag == 0)
    	spec = 1;
   	vec3 specular = light_color * spec;
    
    // attenuation
    float attenuation;
    if (ambientFlag == 1){
    	attenuation = 30;
    } else {
		attenuation = lightParams.x + lightParams.y*distance + lightParams.z*distance_sq;
    }
      
    diffuse /= (attenuation/3);
    specular /= (attenuation/3);
    
    vec3 colorLinear = diffuse + specular;
	vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
	float m = max(max(colorGammaCorrected.x, colorGammaCorrected.y), colorGammaCorrected.z);
	if (m < 0.08) {
		colorGammaCorrected = colorGammaCorrected*colorGammaCorrected;
	}
	
		
	fragColor = vec4(colorGammaCorrected,1.0);
}