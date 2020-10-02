#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex_in[3];

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;

uniform int viewPortId = 0;

void main(){
	for(int i = 0; i < gl_in.length; i++){
		vertex.position = vertex_in[i].position;
		vertex.normal = vertex_in[i].normal;
		vertex.texCoord[0] = vertex_in[i].texCoord[0];
		vertex.texCoord[1] = vertex_in[i].texCoord[1];
		vertex.color = vertex_in[i].color;

		gl_ViewportIndex = viewPortId;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}