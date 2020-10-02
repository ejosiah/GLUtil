#version 450 core
#pragma degub(on)
#pragma optimize(off)
#pragma compile(off)

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

uniform int id = 0;

in ncl_PerVertex{
	smooth vec2 uv;
	smooth float val;
	smooth vec3 normal;
	smooth vec3 position;
} ncl_in[3];

out ncl_PerVertex{
	smooth vec2 uv;
	smooth float val;
	smooth vec3 normal;
};

void main(){
	for(int i = 0; i < 3; i++){
		gl_ViewportIndex = id;
		gl_Position = gl_in[i].gl_Position;

		uv = ncl_in[i].uv;
		val = ncl_in[i].val;
		normal = ncl_in[i].normal;
		EmitVertex();
	}
	EndPrimitive();
}