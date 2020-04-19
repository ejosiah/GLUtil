#version 450 core
#pragma degub(on)
#pragma optimize(off)

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

#pragma include("constants.glsl")
#pragma include("lightModel.glsl")
in VERTEX{
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
	smooth vec3 spotDirection[MAX_LIGHT_SOURCES];
} vertex_in[3];

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
	smooth vec3 spotDirection[MAX_LIGHT_SOURCES];
} vertex_out;

uniform int id;

void main(){
	for(int i = 0; i < gl_in.length(); i++){
		gl_ViewportIndex = 1;
		gl_Position = gl_in[i].gl_Position;
		vertex_out.position = vertex_in[i].position;
		vertex_out.normal = vertex_in[i].normal;
		vertex_out.bitangent = vertex_in[i].bitangent;
		vertex_out.tangent = vertex_in[i].tangent;
		vertex_out.texCoord = vertex_in[i].texCoord;
		vertex_out.color = vertex_in[i].color;
		vertex_out.eyes = vertex_in[i].eyes;

		for(int j = 0; j < numLights; j++){
			vertex_out.lightDirection[j] = vertex_in[i].lightDirection[j];
			vertex_out.spotDirection[j] = vertex_in[i].spotDirection[j];
		}

		EmitVertex();
	}
	EndPrimitive();
}