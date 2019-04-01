#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

layout(location = 0) uniform mat4 mvp;
layout(location = 1) uniform sampler2D heightmap;
layout(location = 2) uniform vec2 terrain_size;
layout(location = 3) uniform float vertical_scaling;


in vec3[] position_es;
in vec3[] color_es;

out vec3 color;

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main(){
 	vec3 world_pos = interpolate3D(position_es[0], position_es[1], position_es[2]);
 	color = interpolate3D(color_es[0], color_es[1], color_es[2]);
 	
	float s = world_pos.x / terrain_size.x;
	float t = world_pos.z / terrain_size.y;
	vec4 tex_val = texture(heightmap, vec2(s,t));
	vec3 pos = vec3(world_pos.x, tex_val.x*vertical_scaling ,world_pos.z);
	gl_Position = mvp*vec4(pos, 1.0);
}