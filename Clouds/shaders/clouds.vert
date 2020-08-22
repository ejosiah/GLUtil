#version 450 core
#pragma debug(on)
#pragma optimize(off)

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

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;


void main(){
	mat4 mLocal = xform * M;
	mat3 NM = transpose(inverse(mat3(mLocal)));
	vertex.normal = normalize(NM * normal);
//	vertex.position = (mLocal * vec4(position, 1)).xyz;
	vertex.position = (xform * vec4(position, 1)).xyz;
	vertex.texCoord[0] = uv;
	vertex.texCoord[1] = uv1;
	vertex.color = color;


	gl_Position = P * V * M * xform * vec4(position, 1);
}