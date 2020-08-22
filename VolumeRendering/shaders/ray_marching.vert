#version 450 core

layout(location = 0) in vec3 position;
layout(location=8) in mat4 xform;


uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

out ncl_PerVertex{
	smooth vec3 pos;
	smooth vec3 uv;
};

void main(){
	pos = position;
	uv = position + vec3(0.5); // from [-0.5, 0.5] to [0, 1]

	gl_Position = MVP * vec4(position, 1);
}