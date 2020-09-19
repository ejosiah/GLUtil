#version 450 core
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 uv1;
layout(location=8) in mat4 xform;

out ncl_PerVertex {
	smooth vec3 local_pos;
};


void main(){
	local_pos = position;
	
	gl_Position = P * V * M * vec4(position, 1);
}