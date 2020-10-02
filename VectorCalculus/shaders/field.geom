#version 450 core 

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int id = 1;

smooth in vec2 v_st[3];
smooth out vec2 st;

void main(){
	for(int i = 0; i < 3; i++){
		gl_ViewportIndex = id;
		gl_Position = gl_in[i].gl_Position;
		st = v_st[i];
		EmitVertex();
	}
	EndPrimitive();
}
