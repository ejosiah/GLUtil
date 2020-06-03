#version 450 core 

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;

out VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vert_out;

uniform mat4 M;

void main(){
	mat3 nform = transpose(inverse(mat3(M * xform)));

	vert_out.position = (M * xform * vec4(position, 1)).xyz;
	vert_out.color = color;
	vert_out.normal = nform * normal;
	vert_out.tangent = nform * tangent;
	vert_out.bitangent = nform * bitangent;
	vert_out.uv = uv;
	
	gl_Position = vec4(vert_out.position, 1.0);
}