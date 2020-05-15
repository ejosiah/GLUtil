#version 450 core 

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int viewId;

in ncl_PerVertex{
	smooth vec3 diffuseColor;
} ncl_in[3];

out vec3 diffuseColor;

void main(){
	for(int i = 0; i < gl_in.length(); i++){
		gl_ViewportIndex = viewId;
		diffuseColor = ncl_in[i].diffuseColor;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}