#version 450 core

layout(location = 0 ) in vec4 initial_position;
layout(location = 1) in vec4 position_offset;

uniform mat4 MVP;

void main(){
	vec3 pos = initial_position.xyz + position_offset.xyz;
	gl_Position = MVP * vec4(pos, 1);
}