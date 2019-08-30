#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location=0) in vec3 position;

void main(){
	int x = 1 << 0;
	gl_Position = vec4(position, 1);
}