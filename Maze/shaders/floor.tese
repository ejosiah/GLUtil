#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(quads, fractional_even_spacing, ccw) in;

#pragma include("lighting.vert.glsl")

layout(binding = 4) uniform sampler2D displacementMap;

uniform bool displace = false;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVP;

uniform float s = 1;	// uv multipler


in VERTEX {
	smooth vec3 position;
	smooth vec4 color;
	smooth mat4 xform;
} v_in[];

void main(){
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float i_u = 1 - u;
	float i_v = 1 - v;

	vec2 uv = vec2(u * s, v * s);

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	vec4 p = p0 * i_u * i_v + 
			 p1 * u * i_v + 
			 p3 * v * i_u +
			 p2 * u * v;

	mat4 xform = v_in[0].xform;
	p = xform * p;

	mat3 nform = inverse(transpose(mat3(xform)));
	vec3 t =  normalize(nform * vec3(1, 0, 0));
	vec3 n = normalize(nform * vec3(0, 1, 0));
	vec3 bi = normalize(nform * vec3(0, 0, 1));
	applyLight(MV, V, p.xyz, n, t, bi);
	
	vertex_out.texCoord = vec2(u * s, v * s);
	vertex_out.color = v_in[0].color;

	gl_Position = MVP * p;
}