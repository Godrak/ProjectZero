#version 450

layout(location = 0) in vec3 vertex_position;
layout(location = 0) uniform mat4 mvp;

struct InstanceData
{
	float size_x; float size_y; float size_y;
	float color_x; float color_y; float color_z;
	float pos_x; float pos_y; float pos_z;
	
};

layout (std430, binding=0) buffer InstanceBuffer
{
	InstanceData instanceData[];
};

out vec3 color;
out vec3 normal;

void main(){
	InstanceData data = instanceData[gl_InstanceID];
	color = vec3(data.color_x,data.color_y, data.color_z);
	normal = vertex_position;
	
	vec3 pos = vertex_position*instanceData[gl_InstanceID].size_x;
	pos += vec3(data.pos_x, data.pos_y, data.pos_z);
	gl_Position = mvp*vec4(pos, 1.0);
}