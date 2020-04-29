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


const int NORMAL_DISTRIBUTION_FUNC = 1 << 0;
const int GEOMETRY_FUNC = 1 << 1;
const int FRENEL = 1 << 2;
const int ALL = NORMAL_DISTRIBUTION_FUNC | GEOMETRY_FUNC | FRENEL;

struct Material{
	vec3 albedo;
	float metalness;
	float roughness;
	vec3 ao;
};

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform int bitfield;

vec3 N(vec3 N, vec3 H, float a){
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1) + 1);
	denom = PI * denom * denom;

	return vec3(nom/denom);
}

vec3 G(vec3 N, vec3 V, float k){
	float NdotV = max(dot(N, V), 0);

	float num = NdotV;
	float denom = NdotV * (1 - k) + k;

	return vec3(num/denom);
}

vec3 F(vec3 h, vec3 v, vec3 f0){
	float HdotV = max(dot(h, v), 0);
	return f0 + (1 - f0) * pow(1 - HdotV, 5);
}

float kDirect(float a){
	float rtVal =  (a + 1) * (a + 1);
	rtVal /= 8;
	return rtVal;
}

float kIbl(float a){
	return (a * a)/2;
}

void main(){
	float roughness = material.roughness;
	float metalness = material.metalness;
	mat4 MV = V * M;
	mat3 NM = transpose(inverse(mat3(MV)));
	vertex.normal = normalize(NM * normal);
	vertex.position = (MV * vec4(position, 1)).xyz;
	vertex.texCoord = uv;
	vertex.color = color;

	vec3 pWorld = (M * vec4(position, 1)).xyz;
	vec3 w0 = normalize(viewPos - pWorld);
	vec3 wi = normalize(lightPos - pWorld);
	vec3 n = transpose(inverse(mat3(M))) * normal;
	vec3 h = normalize(w0 + wi);

	vec3 f = vec3(0);
	if((bitfield & NORMAL_DISTRIBUTION_FUNC) == NORMAL_DISTRIBUTION_FUNC){
		f = N(n, h, roughness);
	}
	if((bitfield & GEOMETRY_FUNC) == GEOMETRY_FUNC){
		float k = kDirect(roughness);
		f = G(n, w0, k) * G(n, wi, k);
	}
	if((bitfield & FRENEL) == FRENEL){
		vec3 f0 = mix(vec3(0.04), vertex.color.xyz, metalness);
		f = F(h, w0, f0);
	}

	vertex.color = vec4(f, 1.0f);


	gl_Position = MVP * vec4(position, 1);
}