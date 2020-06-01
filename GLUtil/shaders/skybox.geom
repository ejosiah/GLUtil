#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int id;

in ncl_PerVertex{
	smooth vec3 texCoord;
} ncl_in[3];

out ncl_PerVertex{
	smooth vec3 texCoord;
};

void main(void)
{
	int n;
	
	for (n = 0; n < gl_in.length(); n++){
		gl_ViewportIndex = id;
		gl_Position = gl_in[n].gl_Position;
		texCoord = ncl_in[n].texCoord;
		EmitVertex();
	}
	EndPrimitive();
}