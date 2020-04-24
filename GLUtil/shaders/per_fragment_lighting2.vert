#version 450 core
#pragma debug(on)
#pragma optimize(off)
#pragma storeIntermediate(on)
#pragma include("lighting.vert.glsl")

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 uv1;
layout(location=8) in mat4 xform;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVP;
uniform bool uvMappedToSize;
uniform bool useXform;


void main(){
	
	vec4 pos = vec4(position, 1);
	pos = useXform ? xform * pos : pos;
	vec3 n = useXform ? mat3(xform) * normal : normal;
	vec3 t = useXform ? mat3(xform) * tangent : tangent;
	vec3 bi = useXform ? mat3(xform) * bitangent : bitangent;
	applyLight(MV, V, pos.xyz, n, t, bi);
	vertex_out.texCoord = uvMappedToSize ? uv1 : uv;
	vertex_out.color = color;
	gl_Position = MVP * pos;
}