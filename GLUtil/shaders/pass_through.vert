#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=5) in vec4 color;
smooth out vec4 fColor;

void main(){
	fColor = color;
	gl_Position = MVP * vec4(position, 1);
}