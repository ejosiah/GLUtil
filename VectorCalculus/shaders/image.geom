#version 450 core
#pragma degub(on)
#pragma optimize(off)

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

uniform int id;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex_in[];

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex_out;


void main(){
	for(int n = 0; n < gl_in.length(); n++){
		gl_ViewportIndex = id;
		mat4 MV = V * M;
		mat3 NM = transpose(inverse(mat3(MV)));
		vertex_out.normal = normalize(NM * vertex_in[n].normal);
		vertex_out.position = (MV * vec4(vertex_in[n].position, 1)).xyz;
		vertex_out.texCoord = vertex_in[n].texCoord;
		vertex_out.color = vertex_in[n].color;

		gl_Position = P * vec4(vertex_out.position, 1);;
		EmitVertex();
	}
	EndPrimitive();
}