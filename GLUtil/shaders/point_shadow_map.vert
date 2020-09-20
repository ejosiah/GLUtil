#version 450 core 

layout(location = 0) in vec3 position;
layout(location=8) in mat4 xform;

uniform mat4 M;

smooth out vec3 vlocalPos;

void main(){
	vlocalPos = position;
	gl_Position = M * xform * vec4(position, 1);
}