#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;



in ncl_PerVertex{
	smooth vec2 texCoord;
} ncl_in[3];

smooth out vec2 texCoord;
smooth out vec4 o_color;

uniform int layer;
uniform vec4 color;

void main(){
	for(int i = 0; i < 3; i++){
		gl_Layer = layer;
		vec4 worldPos = gl_in[i].gl_Position;

		texCoord = ncl_in[i].texCoord;
		o_color = color;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}