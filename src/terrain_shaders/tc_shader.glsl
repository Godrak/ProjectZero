#version 450

layout (vertices = 3) out;

layout(location = 2) uniform vec2 terrain_size;
layout (location = 4) uniform vec3 camera_position;

in vec3[] position;
in vec3[] color;

out vec3[] position_es;
out vec3[] color_es;

float getTesLevel(float avg_distance){
	float t_size = (terrain_size.x+terrain_size.y)/2;
	return min(t_size/(1+avg_distance*3),8);
}

void main(){
	position_es[gl_InvocationID] = position[gl_InvocationID];
	color_es[gl_InvocationID] = color[gl_InvocationID];
	
	float dis0 = distance(camera_position.xz, position_es[0].xz);
	float dis1 = distance(camera_position.xz, position_es[1].xz);
	float dis2 = distance(camera_position.xz, position_es[2].xz);
	float avg0 = (dis0 + dis1)/2;
	float avg1 = (dis1 + dis2)/2;
	float avg2 = (dis2 + dis0)/2;
	
		
	gl_TessLevelOuter[0] = getTesLevel(avg1);
    gl_TessLevelOuter[1] = getTesLevel(avg2);
    gl_TessLevelOuter[2] = getTesLevel(avg0);
    gl_TessLevelInner[0] = getTesLevel((dis0+dis1+dis2)/3);
}