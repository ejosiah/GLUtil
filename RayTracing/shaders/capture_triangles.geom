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
	vec4 n0;
	vec4 n1;
	vec4 n2;
	vec4 t0;
	vec4 t1;
	vec4 t2;
	vec4 bi0;
	vec4 bi1;
	vec4 bi2;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
	int id;
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
	shading.n0 = vec4(ncl_in[0].normal, 0);
	shading.n1 = vec4(ncl_in[1].normal, 0);
	shading.n2 = vec4(ncl_in[2].normal, 0);

	shading.t0 = vec4(ncl_in[0].tangent, 0);
	shading.t1 = vec4(ncl_in[1].tangent, 0);
	shading.t2 = vec4(ncl_in[2].tangent, 0);

	shading.bi0 = vec4(ncl_in[0].bitangent, 0);
	shading.bi1 = vec4(ncl_in[1].bitangent, 0);
	shading.bi2 = vec4(ncl_in[2].bitangent, 0);

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