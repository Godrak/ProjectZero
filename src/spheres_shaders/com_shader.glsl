#version 450 core

layout (local_size_x = 1) in;

layout(binding = 0) uniform sampler2D heightmap;
layout(r32ui, binding = 1) uniform uimage2D deformation_texture;

layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 5) uniform vec3 gravity;
layout(location = 6) uniform float time_delta;
layout(location = 8) uniform float pixel_resolution;

struct InstanceData
{
	float size_x; float size_y; float size_y;
	float color_x; float color_y; float color_z;
	float pos_x; float pos_y; float pos_z;
	float speed_x; float speed_y; float speed_z;
	float vel_x; float vel_y; float vel_z;
	float artists_scale;
};

layout (std430, binding=0) buffer InstanceBuffer
{
	InstanceData instanceData[];
};

void writePos(vec3 new_pos){
	instanceData[gl_GlobalInvocationID.x].pos_x = new_pos.x;
	instanceData[gl_GlobalInvocationID.x].pos_y = new_pos.y;
	instanceData[gl_GlobalInvocationID.x].pos_z = new_pos.z;
}

void writeVelocity(vec3 new_velocity){
	instanceData[gl_GlobalInvocationID.x].vel_x = new_velocity.x;
	instanceData[gl_GlobalInvocationID.x].vel_y = new_velocity.y;
	instanceData[gl_GlobalInvocationID.x].vel_z = new_velocity.z;
}


float getGroundHeight(float world_x, float world_z){
	float s = world_x / terrain_size.x;
	float t = world_z / terrain_size.y;
	vec4 height = texture(heightmap, vec2(s,t));
	return height*vertical_scaling;
}

//gl_GlobalInvocationID - x component defines the sphere we are computing

void physics(){
	InstanceData data = instanceData[gl_GlobalInvocationID.x];
	vec3 pos = vec3(data.pos_x, data.pos_y, data.pos_z);
	vec3 vel = vec3(data.vel_x, data.vel_y, data.vel_z);

	pos += time_delta*vel;
	
	float diff = 0.1;
	float s = instanceData[gl_GlobalInvocationID.x].pos_x / terrain_size.x;
	float t = instanceData[gl_GlobalInvocationID.x].pos_z / terrain_size.y;
	float tex_val = texture(heightmap, vec2(s,t)).x;
	float tex_s = texture(heightmap, vec2(s+diff,t)).x;
	float tex_ss = texture(heightmap, vec2(s-diff,t)).x;
	float tex_t = texture(heightmap, vec2(s,t+diff)).x;
	float tex_tt = texture(heightmap, vec2(s,t-diff)).x;
	
	vec3 gradient = vec3( (tex_s - tex_ss) / (2*diff), 0 , (tex_t-tex_tt)/ (2*diff));
	vec3 dir = -gradient;
	
	float height = tex_val.x*vertical_scaling;
	
	if (pos.y - data.size_y < height+0.3){
		//vel.y = 1;
		pos.y = (height + data.size_y);
		vel += dir*10;
	}
	
	vel += time_delta*gravity;
	float l = length(vel);
	vel = normalize(vel)*min(l,200);
	
	if (s < 0 || s > 1 || t < 0 || t > 1)
		pos = vec3 (terrain_size.x/2, terrain_size.x, terrain_size.y/2);

	writePos(pos);
	writeVelocity(vel);
	
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
		ivec2 tex_coords = ivec2(round( point*pixel_resolution ));
		return tex_coords;
	} else {
		return ivec2(-1,-1);
	}
}

void deformSnow(){
	InstanceData data = instanceData[gl_GlobalInvocationID.x];
	vec3 world_pos = vec3(data.pos_x, data.pos_y, data.pos_z);
	float deform_point_height = world_pos.y-data.size_y/2;
	vec2 world_texture_size = imageSize(deformation_texture)/pixel_resolution;
	vec2 deform_point_loc = world_pos.xz;
	
	float mc = 20; // half the side of area of units covered
	for (int x = -int(round(mc*pixel_resolution)); x < mc*pixel_resolution; x++){ // units * pixel_res (per unit) = pixels
	for (int y = -int(round(mc*pixel_resolution)); y < mc*pixel_resolution; y++){
		vec2 point_delta = vec2(x,y)/pixel_resolution;
		vec2 point_loc = deform_point_loc + point_delta;
	
		float point_distance = length(point_delta);
		float deformation_height = deform_point_height + point_distance*point_distance*data.artists_scale;
		
		ivec2 tex_coords = pointToTextureUV(point_loc, world_texture_size);
		
		if (tex_coords.x > -1){
			uint def = uint(round(deformation_height)) << 16;
			uint height = uint(round(deform_point_height));
			uint res = def | height;
			imageAtomicMin(deformation_texture, tex_coords, res);
		}
			
	}
	}
	
}

void main(){
	physics();
	
	deformSnow();
	
}






