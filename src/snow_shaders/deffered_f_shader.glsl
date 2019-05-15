#version 450

layout(binding = 0) uniform sampler2D heightmap;
layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 8) uniform float pixel_resolution;
layout(location = 9) uniform float snow_height;
layout(location = 10) uniform float normal_off;

in vec3	world_position;
in vec3 diffuse_color;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;


float PHI = 1.61803398874989484820459 * 00000.1; // Golden Ratio
float PI  = 3.14159265358979323846264 * 00000.1; // PI
float SQ2 = 1.41421356237309504880169 * 10000.0; // Square Root of Two
float seed = 10;

float get_noise(float x,float y,float z)
{
    return fract(sin(dot(vec3(x,y,z)*(seed+PHI), vec3(PHI, PI, PHI)))*SQ2);
}


float getGroundHeight(vec2 world_pos){
	vec2 uv = world_pos / terrain_size;
	vec4 height = texture(heightmap, uv);
	return height.x*vertical_scaling;
}

// return 1 if v inside the box, return 0 otherwise
float insideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {
    vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;   
}

vec2 wrap (vec2 world_pos, vec2 texture_size){
	return world_pos-(texture_size*floor ( world_pos / texture_size));
}

ivec2 pointToTextureUV(vec2 point, vec2 world_texture_size){
	vec2 camera_delta = point - camera_position.xz;
	
	if (insideBox(camera_delta, -world_texture_size/2 + 1, world_texture_size/2 -1)==1){
		vec2 point = wrap(point, world_texture_size);
		ivec2 tex_coords = ivec2(floor( point*pixel_resolution ));
		return tex_coords;
	} else {
		return ivec2(-1,-1);
	}
}

float computeDepressionOrElevation(float deformation_height, float deform_point_height, float total_height){
	if (deform_point_height > total_height || 
		total_height - deformation_height > 2*snow_height || 
		total_height - deform_point_height > 2*snow_height ||
		total_height - deform_point_height <= 0)
		return total_height;
	
	if (deformation_height < total_height)
		return deformation_height;

	float depression_distance = sqrt(max(total_height - deform_point_height,0));
	float distance_from_foot = sqrt(max(deformation_height - deform_point_height,0));
	
	float depression_depth = abs(total_height - deform_point_height);
	if (depression_depth < 10) return total_height;
	 
	float elevation_distance = distance_from_foot - depression_distance;
	float maximum_elevation_distance  = depression_depth*0.1;	
	float ratio = abs(elevation_distance / maximum_elevation_distance);
	
	float height = maximum_elevation_distance*5;
	float ratio6 = ratio*ratio*ratio*ratio*ratio*ratio;
	float t = 2*sqrt((ratio)/(1+10*ratio6)) / (1+10*ratio*ratio);
	
	float elevation = t*height;
	return total_height + elevation;
}

float getDeformedHeight(vec2 world_pos){
	float ground_height = getGroundHeight(world_pos);
	float height = ground_height+snow_height;
	
	vec2 world_texture_size = imageSize(deformation_texture)/pixel_resolution;
	ivec2 tex_coords = pointToTextureUV(world_pos, world_texture_size);
	
	float deformation_height = height;
	if (tex_coords.x > -1){
	
		uint res = uint(imageLoad(deformation_texture,tex_coords));
		uint deform_height = res >> 16;
		uint deform_point_height = (res << 16) >> 16;
		
		deformation_height = computeDepressionOrElevation(deform_height, deform_point_height, height);
	}

	return deformation_height;
}


vec2 getRandomNormalOffset(vec2 data){
	float noise = get_noise(data.x, data.y, 1.235);
	return vec2(normal_off+noise*3, 0);
}

vec3 getGroundNormal(vec2 world_pos){
	vec2 right = world_pos+getRandomNormalOffset(world_pos);
	vec2 left = world_pos-getRandomNormalOffset(right);
	vec2 down = world_pos-getRandomNormalOffset(left);
	vec2 up = world_pos+getRandomNormalOffset(down);
	
	float r_height = getDeformedHeight(right);
	float l_height = getDeformedHeight(left);
	float u_height = getDeformedHeight(up);
	float d_height = getDeformedHeight(down);
	
	vec3 tangent = vec3(2*normal_off, r_height - l_height, 0.0);
	vec3 bitangent = vec3(0.0, d_height - u_height, 2*normal_off);
	
	vec3 normal = normalize(-cross(tangent, bitangent));
	return normal;
}

void main(){
	position = world_position;
	normal = getGroundNormal(world_position.xz);
	color = diffuse_color;
}