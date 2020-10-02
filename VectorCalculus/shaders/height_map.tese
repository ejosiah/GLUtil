#version 450 core
#pragma compile(off)

layout (quads, equal_spacing, ccw) in;

layout(binding = 0) uniform sampler3D heightMap;
layout(binding = 1) uniform sampler3D gradiantMap;

uniform mat4 MVP;
uniform mat4 M;
uniform int slice;
uniform int numSlices;


out ncl_PerVertex{
	smooth vec2 uv;
	smooth float val;
	smooth vec3 normal;
	smooth vec3 position;
};

void main(){
	uv = gl_TessCoord.xy;
	float u = uv.x;
	float v = uv.y;
	float i_u = 1 - u;
	float i_v = 1 - v;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	vec4 p = p0 * i_u * i_v + 
			 p1 * u * i_v + 
			 p3 * v * i_u +
			 p2 * u * v;

	float z = float(slice)/float(numSlices);
	val = texture(heightMap, vec3(uv, z)).r;
	normal = texture(gradiantMap, vec3(uv, z)).rgb * 2 - 1;
	p.y = val;
	position = p.xyz;
	gl_Position = MVP * p;
}