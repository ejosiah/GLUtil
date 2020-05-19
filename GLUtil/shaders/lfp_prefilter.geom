#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform int layer;
uniform mat4 views[6];
uniform mat4 projection;

out smooth vec3 texCoord;

void main(){
	for(int face = 0; face < 6; face++){
		gl_Layer = layer * 6 + face;
		for(int i = 0; i < gl_in.length(); i++){
			texCoord = gl_in[i].gl_Position.xyz;
			gl_Position = projection * views[face] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}