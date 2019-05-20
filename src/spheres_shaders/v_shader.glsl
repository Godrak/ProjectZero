#version 450

layout(location = 0) in vec3 vertex_position;
layout(location = 0) uniform mat4 mvp;

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

out vec3 diffuse_color;
out vec3 model_position;
out vec3 world_position;

void main(){
	InstanceData data = instanceData[gl_InstanceID];
	diffuse_color = vec3(data.color_x,data.color_y, data.color_z);
	model_position = vertex_position.xzy; 
	
	vec3 pos = model_position*instanceData[gl_InstanceID].size_x;
	pos += vec3(data.pos_x, data.pos_y, data.pos_z);
	world_position = pos;
	
	gl_Position = mvp*vec4(pos, 1.0);
}