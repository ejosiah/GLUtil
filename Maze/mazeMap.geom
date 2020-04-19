#version 450 core
#pragma degub(on)
#pragma optimize(off)

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

uniform int id;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex_in[];

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex_out;


void main(){
	for(int i = 0; i < gl_in.length(); i++){
		gl_ViewportIndex = 0;
		gl_Position = gl_in[i].gl_Position;
		vertex_out.position = vertex_in[i].position;
		vertex_out.color = vertex_in[i].color;
		EmitVertex();
	}
	EndPrimitive();
}