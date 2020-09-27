#version 450 core

uniform mat4 M;

uniform mat4 MVP;

layout(location=0) in vec3 position;

smooth out vec3 world_pos;

void main(){
	world_pos = (M * vec4(position, 1)).xyz;
	gl_Position = MVP * vec4(position, 1);
}