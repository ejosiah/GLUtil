#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 22.0/7.0

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

uniform vec3 lightPos;
uniform float alpha;

float ndf(vec3 N, vec3 H, float a){
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1) + 1);
	denom = PI * denom * denom;

	return nom/denom;
}

void main(){
	mat4 MV = V * M;
	mat3 NM = transpose(inverse(mat3(MV)));
	vertex.normal = normalize(NM * normal);
	vertex.position = (MV * vec4(position, 1)).xyz;
	vertex.texCoord = uv;
	vertex.color = color;

	vec3 pWorld = (M * vec4(position, 1)).xyz;
	vec3 w0 = -(V * vec4(pWorld, 1)).xyz;
	vec3 wi = lightPos - pWorld;
	vec3 n = transpose(inverse(mat3(M))) * normal;
	vec3 h = normalize(w0 + wi);

	float alphaSqr = alpha * alpha;
	float nDoth = max(dot(n, h), 0);

	float ndf = alphaSqr;
	float denom = PI * pow(nDoth * nDoth * (alphaSqr - 1) + 1, 2);

	ndf /= denom;

	vertex.color = vec4(ndf, ndf, ndf, 1.0f);


	gl_Position = MVP * vec4(position, 1);
}