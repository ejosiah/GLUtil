#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;


uniform mat4 projection;
uniform mat4 view[6];

out vec4 fragPos;

void main(){
	for(int face = 0; face < 6; face++){
		gl_Layer = face;
		for(int i = 0; i < 3; i++){
			fragPos = gl_in[i].gl_Position;
			gl_Position = projection * view[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}