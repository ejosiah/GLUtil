#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 MVP;
uniform float normal_length = 1;

smooth in vec2 uv[3];
smooth in vec3 worldPos[3];

vec3 calcNormal(){
	vec3 v0 = worldPos[0];
	vec3 v1 = worldPos[1];
	vec3 v2 = worldPos[2];

	vec3 A = v1 - v0;
	vec3 B = v2 - v0;
	vec3 N = cross(A, B);
	return normalize(N);
}


void main(){
	vec3 v0 = worldPos[0];
	vec3 v1 = worldPos[1];
	vec3 v2 = worldPos[2];

	vec3 normal = calcNormal();

	vec3 center = (v0 + v1 + v2) * 0.333333333;
	vec3 tip =  center + normal * normal_length;

	gl_Position = MVP * vec4(center, 1);
	EmitVertex();

	gl_Position = MVP * vec4(tip, 1);
	EmitVertex();
	EndPrimitive();
}