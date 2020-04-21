#version 450 core
#pragma degub(on)
#pragma optimize(off)

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

#pragma include("constants.glsl")
#pragma include("lightModel.glsl")
#pragma include("vertex_in_array.glsl")
#pragma include("vertex_out.glsl")

uniform int id;

void main(){
	for(int i = 0; i < gl_in.length(); i++){
		gl_ViewportIndex = id;
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