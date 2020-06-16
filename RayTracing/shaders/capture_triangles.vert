#version 450 core
#pragma debug(on)
#pragma optimize(off)


layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;

out ncl_PerVetex {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
} ncl_out;


void main(){
	vec4 worldPos = xform * vec4(position, 1.0);
	mat3 NM = transpose(inverse(mat3(xform)));
	ncl_out.normal = normalize(NM * normal);
	ncl_out.tangent = normalize(NM * tangent);
	ncl_out.bitangent = normalize(NM * bitangent);
	ncl_out.position = worldPos.xyz;
	ncl_out.texCoord = uv;
	ncl_out.color = color;

	gl_Position = worldPos;
}