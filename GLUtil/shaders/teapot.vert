#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;

out VERTEX{
	smooth vec4 color;
} out_vertex;

void main(){
	out_vertex.color = color;
	gl_Position = vec4(position, 1);
}