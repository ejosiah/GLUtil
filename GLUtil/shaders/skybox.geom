#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int id;

smooth in vec3 texCoord[3];
smooth out vec3 textCoord_out;

void main(void)
{
	int n;
	
	for (n = 0; n < gl_in.length(); n++){
		gl_ViewportIndex = id;
		gl_Position = gl_in[n].gl_Position;
		textCoord_out = texCoord[n];
		EmitVertex();
	}
	EndPrimitive();
}