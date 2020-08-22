#version 450 core
#pragma compile(on)

layout(triangles) in;
layout(triangle_strip, max_vertices= 3) out;

in ncl_PerVertex{
	int instanceId;
	smooth vec2 uv;
} ncl_in[3];

out ncl_PerVertex{
	smooth vec2 uv;
	flat int slice;
};


void main(){
	int id = ncl_in[0].instanceId;
	for(int i = 0; i < 3; i++){
		gl_Layer =  id;
		uv = ncl_in[i].uv;
		slice = gl_Layer;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}