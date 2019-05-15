#version 450

layout(location = 0) in vec3 vertex_position;
layout(location = 0) uniform mat4 mvp;
layout(location = 14) uniform vec3 lightParams; //Kc, Kl, Kq

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

out vec3 light_color;
out vec3 light_center;
out float light_size;

void main(){
	InstanceData data = instanceData[gl_InstanceID];
	light_color = vec3(data.color_x,data.color_y, data.color_z);
	light_center = vec3(data.pos_x, data.pos_y, data.pos_z);
	light_size = length(vertex_position*instanceData[gl_InstanceID].size_x);
	
	float m = max(max(light_color.x, light_color.y), light_color.z);
	float Kc = lightParams.x;
	float Kl = lightParams.y;
	float Kq = lightParams.z;
	float radius = (  -Kl + sqrt( Kl*Kl - 4*Kq*(Kc - m*(256/5.0) ))  )/(2*Kq);
	vec3 pos = vertex_position*radius;
	
	pos += light_center;
	
	gl_Position = mvp*vec4(pos, 1.0);
}