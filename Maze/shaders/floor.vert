#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;
layout(location=8) in mat4 xform;

out VERTEX {
	smooth vec3 position;
	smooth vec4 color;
	smooth mat4 xform;
} v_out;

void main(){
	vec4 p = vec4(position, 1);
	v_out.position = p.xyz;
	v_out.color = color;
	v_out.xform = xform;

	gl_Position = p;
}
