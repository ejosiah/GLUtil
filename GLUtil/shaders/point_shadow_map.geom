#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;


uniform mat4 projection;
uniform mat4 view[6];

in vec3 vlocalPos[3];


out ncl_PerVertexPos{
	smooth vec3 local;
	smooth vec3 world;
	smooth vec3 view;
} pos;

void main(){
	for(int face = 0; face < 6; face++){
		gl_Layer = face;
		for(int i = 0; i < 3; i++){
			vec4 worldPos = gl_in[i].gl_Position;
			vec4 viewPos = view[face] * worldPos;

			pos.local = vlocalPos[i];
			pos.world = worldPos.xyz;
			pos.view = viewPos.xyz;

			gl_Position = projection * viewPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}