#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10

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

struct Light{
	vec3 position;
	vec3 intensity;
};

layout(std430, binding=0) buffer SCENE_SSBO{
	vec3 eyes;
	Light lights[MAX_SCENE_LIGHTS];
};

uniform int numLights = 6;

out VERTEX{
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_out;


void main(){
	
	mat3 nform = inverse(transpose(mat3(M * xform)));
	vec3 n = nform * normal;
	vec3 t = nform * tangent;
	vec3 b = nform * bitangent;
	vertex_out.normal = n;
	vertex_out.tangent = t;
	vertex_out.bitangent = b;

	mat3 olm = mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);

	vec4 worldPos = M * xform * vec4(position, 1.0);

	vertex_out.position = worldPos.xyz;
	
	vertex_out.uv = uv;

	gl_Position = P * V * worldPos;
}