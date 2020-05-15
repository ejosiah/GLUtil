#version 450 core 

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int viewId;

in ncl_PerVertex{
	smooth vec3 texCoord;
	smooth vec2 st;
} ncl_in[3];

smooth out vec3 texCoord;

void main(){
	for(int i = 0; i < gl_in.length(); i++){
		gl_ViewportIndex = viewId;
		gl_Position = gl_in[i].gl_Position;
		texCoord = ncl_in[i].texCoord;
		EmitVertex();
	}
	EndPrimitive();
}