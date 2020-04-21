#version 450 core
#pragma degub(on)
#pragma optimize(off)

layout(lines) in;

layout(line_strip, max_vertices = 2) out;

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
		gl_ViewportIndex = id;
		gl_Position = gl_in[i].gl_Position;
		vertex_out.position = vertex_in[i].position;
		vertex_out.color = vertex_in[i].color;
		vertex_out.texCoord = vertex_in[i].texCoord;
		vertex_out.normal = vertex_in[i].normal;
		EmitVertex();
	}
	EndPrimitive();
}