#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;

smooth out vec4 vcolor;

void main(){
	vcolor = color;
	gl_Position = vec4(position, 1);
}