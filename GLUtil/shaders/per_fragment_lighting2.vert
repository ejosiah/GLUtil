#version 450 core
#pragma debug(on)
#pragma optimize(off)

#pragma include("lighting.vert.glsl")

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 uv1;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVP;
uniform bool uvMappedToSize;


void main(){
	
	applyLight(MV, V, position, normal, tangent, bitangent);
	vertex_out.texCoord = uvMappedToSize ? uv1 : uv;
	vertex_out.color = color;
	gl_Position = MVP * vec4(position, 1);
}