#version 450 core

layout (local_size_x = 1) in;

layout(location = 1) uniform sampler2D heightmap;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;
layout(location = 4) uniform vec3 camera_position;
layout(location = 5) uniform vec3 gravity;
layout(location = 6) uniform float time_delta;

struct InstanceData
{
	float size_x; float size_y; float size_y;
	float color_x; float color_y; float color_z;
	float pos_x; float pos_y; float pos_z;
	float speed_x; float speed_y; float speed_z;
	float vel_x; float vel_y; float vel_z;
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

void main(){
	InstanceData data = instanceData[gl_GlobalInvocationID.x];
	vec3 pos = vec3(data.pos_x, data.pos_y, data.pos_z);
	vec3 vel = vec3(data.vel_x, data.vel_y, data.vel_z);

	vel += clamp(time_delta*gravity, -vec3(30), vec3(30));
	pos += time_delta*vel;
	
	float diff = 0.05;
	float s = instanceData[gl_GlobalInvocationID.x].pos_x / terrain_size.x;
	float t = instanceData[gl_GlobalInvocationID.x].pos_z / terrain_size.y;
	float tex_val = texture(heightmap, vec2(s,t)).x;
	float tex_s = texture(heightmap, vec2(s+diff,t)).x;
	float tex_ss = texture(heightmap, vec2(s-diff,t)).x;
	float tex_t = texture(heightmap, vec2(s,t+diff)).x;
	float tex_tt = texture(heightmap, vec2(s,t-diff)).x;
	
	vec3 dir = normalize(vec3(tex_ss - tex_s, 0, tex_tt - tex_t));
	float height = tex_val.x*vertical_scaling;
	
	if (pos.y - data.size_y < height-0.5){
		pos.y = (height + data.size_y)*1/3 + pos.y*2/3;
		vel.y = 0;
		vel += dir*0.5;
		vel = vel - vel*0.1*time_delta;
	}
	
	if (s < 0 || s > 1 || t < 0 || t > 1)
		pos = vec3 (terrain_size.x/2, terrain_size.x, terrain_size.y/2);

	writePos(pos);
	writeVelocity(vel);
}






