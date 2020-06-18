#version 450 core


//layout(triangles_adjacency) in;
//layout(triangle_strip, max_vertices = 6) out;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(binding = 1, offset = 0) uniform atomic_uint next_index;

uniform bool genTangents = false;
uniform int objectToWorldId = 0;
uniform int worldToObjectId = 0;
uniform int materialId = -1;


struct Triangle {
	vec4 a;
	vec4 b;
	vec4 c;
	int objectToWorldId;
	int worldToObjectId;
	int id;
	int matId;
};


struct Shading {
	vec3 n0;
	int id;
	vec3 n1;
	vec3 n2;
	vec3 t0;
	vec3 t1;
	vec3 t2;
	vec3 bi0;
	vec3 bi1;
	vec3 bi2;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
};

in ncl_PerVetex {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
} ncl_in[3];

layout(std430, binding=6) buffer TRIANGLE_SSB0{
	Triangle triangles[];
};

layout(std430, binding=7) buffer SHADING_SSB0{
	Shading shadings[];
};


out ncl_PerVetex {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
} ncl_out;


void main(){
	uint id = atomicCounterIncrement(next_index);
	Triangle triangle;
	triangle.a = gl_in[0].gl_Position;
	triangle.b = gl_in[1].gl_Position;
	triangle.c = gl_in[2].gl_Position;
	triangle.id = int(id);
	triangle.objectToWorldId = objectToWorldId;
	triangle.worldToObjectId = worldToObjectId;
	triangle.matId = materialId;

	Shading shading;
	shading.n0 = ncl_in[0].normal;
	shading.n1 = ncl_in[1].normal;
	shading.n2 = ncl_in[2].normal;

	shading.t0 = ncl_in[0].tangent;
	shading.t1 = ncl_in[1].tangent;
	shading.t2 = ncl_in[2].tangent;

	shading.bi0 = ncl_in[0].bitangent;
	shading.bi1 = ncl_in[1].bitangent;
	shading.bi2 = ncl_in[2].bitangent;

	shading.uv0 = ncl_in[0].texCoord;
	shading.uv1 = ncl_in[1].texCoord;
	shading.uv2 = ncl_in[2].texCoord;
	shading.id = int(id);

	triangles[int(id)] = triangle;
	shadings[int(id)] = shading;

	for(int i = 0; i < gl_in.length(); i++){
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}