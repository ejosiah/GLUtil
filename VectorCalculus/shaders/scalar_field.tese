#version 450 core

layout (quads, equal_spacing, ccw) in;

layout(binding = 0) uniform sampler2D heightMap;

uniform mat4 MVP;
uniform mat4 M;

smooth out vec2 uv;
smooth out float val;

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

	val = texture(heightMap, uv).r;
	val = p.y;
	gl_Position = MVP * p;
}