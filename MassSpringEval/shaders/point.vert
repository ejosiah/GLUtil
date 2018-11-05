#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;
smooth out vec4 fColor;

void main(){
	fColor = color;
	gl_Position = P * V * M * vec4(position, 1);
}