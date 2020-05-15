#version 450 core
#pragma debug(on)
#pragma optimize(off)
layout(binding=0) uniform samplerBuffer coeffs;


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location=5) in vec2 uv;
layout(location = 8) in mat4 xform;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;



const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

const vec3 L00 = texelFetch(coeffs, 0).rgb;
const vec3 L1_1 = texelFetch(coeffs, 1).rgb;
const vec3 L10 = texelFetch(coeffs, 2).rgb;
const vec3 L11 = texelFetch(coeffs, 3).rgb;
const vec3 L2_2 = texelFetch(coeffs, 4).rgb;
const vec3 L2_1 = texelFetch(coeffs, 5).rgb;
const vec3 L20 = texelFetch(coeffs, 6).rgb;
const vec3 L21 = texelFetch(coeffs, 7).rgb;
const vec3 L22 = texelFetch(coeffs, 8).rgb;


out ncl_PerVertex{
	smooth vec3 diffuseColor;
};


void main(){
	
	mat3 nform = transpose(inverse(mat3(M * xform)));
	vec3 n = nform * normal;

	diffuseColor = C1 * L22 * (n.x * n.x - n.y * n.y);
	diffuseColor += C3 * L20 * n.z * n.z;
	diffuseColor += C4 * L00 - C5 * L20;
	diffuseColor += 2 * C1 * (L2_2 * n.x * n.y + L21 * n.x * n.y + L2_1 * n.y * n.z);
	diffuseColor += 2 * C2 * (L11 * n.x + L1_1 * n.y + L10 * n.z);
	
	vec4 worldPos = M * xform * vec4(position, 1.0);
	gl_Position = P * V * worldPos;
}