#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;


uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;
uniform mat4 lightSpaceView;
uniform int numLights = 6;

out VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_out;


void main(){
	
	mat3 nform = transpose(inverse(mat3(M * xform)));
	vec3 n = nform * normal;
	vec3 t = nform * tangent;
	vec3 b = nform * bitangent;
	vertex_out.normal = n;
	vertex_out.tangent = t;
	vertex_out.bitangent = b;
	vertex_out.color = color;


	vec4 localPos = xform * vec4(position, 1.0);
	vec4 worldPos = M * localPos;
	vertex_out.position = worldPos.xyz;
	vertex_out.lightSpacePos = lightSpaceView * vec4(worldPos.xyz, 1);
	
	vertex_out.uv = uv;

	gl_Position = P * V * worldPos;
}