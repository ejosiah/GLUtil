#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;

out VERTEX {
	smooth vec3 position;
	smooth vec4 color;
} v_out;

uniform bool useXform;

void main(){
	v_out.position = position;
	v_out.color = color;

	gl_Position = vec4(position, 1);
}
