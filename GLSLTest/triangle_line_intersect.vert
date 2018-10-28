#version 450 core
#pragma debug(on)
#pragma optimize(off)
#pragma include("triangle_ray_intersection.glsl")

layout(location = 0) in vec4 position;

layout(binding=1) uniform samplerBuffer testData;

out vec3 test_output;
out vec4 extra_data;

void main(){
	vec3 a = texelFetch(testData, 0).xyz;
	vec3 b = texelFetch(testData, 1).xyz;
	vec3 c = texelFetch(testData, 2).xyz;
	vec3 p = texelFetch(testData, 3).xyz;
	vec3 q = texelFetch(testData, 4).xyz;

	float u = 0.0;
	float v = 0.0;
	float w = 0.0;
	float t = 0.0;
	float result = triangleRayIntersect(a, b, c, p, q, true, u, v, w, t) ? 1.0 : 0.0;

	gl_Position = position;
	test_output = vec3(result);
	extra_data = vec4(u, v, w, t);
}