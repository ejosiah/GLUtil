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
layout(location=8) in mat4 xform;

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

layout(xfb_buffer=0, xfb_offset=0) out vec3 capture_position;

void main(){
	mat4 MV = V *  xform * M;
	mat3 NM = transpose(inverse(mat3(MV)));
	vertex.normal = normalize(NM * normal);
	vertex.position = (MV * vec4(position, 1)).xyz;
	vertex.texCoord = uv;
	vertex.color = color;

	capture_position = (M * xform * vec4(position, 1.0)).xyz;

	gl_Position = MVP * xform * vec4(position, 1);
}