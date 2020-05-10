#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;


uniform mat4 projection;
uniform mat4 views[6];
uniform mat4 lightSpaceView;

in VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vert_in[];

out VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vert_out;

void main(){
	for(int face = 0; face < 6; face++){
		gl_Layer = face;
		for(int i = 0; i < 3; i++){
			vec4 worldPos = gl_in[i].gl_Position;

			vert_out.position = worldPos.xyz;
			vert_out.normal = vert_in[i].normal;
			vert_out.tangent = vert_in[i].tangent;
			vert_out.bitangent = vert_in[i].bitangent;
			vert_out.color = vert_in[i].color;
			vert_out.uv = vert_in[i].uv;
			vert_out.lightSpacePos = lightSpaceView * worldPos;

			gl_Position = projection * views[face] * worldPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}