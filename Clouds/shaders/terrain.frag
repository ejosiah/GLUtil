#version 450 core

layout(binding = 0) uniform sampler2D heightMap;
layout(binding = 1) uniform sampler2D normalMap;

uniform bool generated;
uniform vec3 camPos;
uniform vec3 sunPos;

in ncl_PerVertex{
	vec3 worldPos;
	vec3 normal;
	vec2 uv;
	bool isNormal;
};

const vec3 terrainColor = vec3(0.5, 0.25, 0.02);
const vec3 Intensity = vec3(1);

out vec4 fragColor;

vec3 getNormal(){
	vec3 tNormal = texture(normalMap, uv).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(worldPos);
	vec3 Q2 = dFdy(worldPos);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N = normalize(normal);
	//vec3 N = normalize(vec3(0, 1, 0));
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(tNormal);
}

void main(){
	vec3 L = normalize(sunPos - worldPos);
	vec3 V = normalize(camPos - worldPos);
	vec3 N = getNormal();
	vec3 E = normalize(L + V);

	vec3 ambient = 0.3 * terrainColor;
	vec3 diffuse = max(0, dot(L, N)) * terrainColor;
	vec3 specular =pow(max(0, dot(E, N)), 50) * terrainColor;

	fragColor.a = 1;
	fragColor.rgb = Intensity * (ambient + diffuse + specular);
	//fragColor.rgb = texture(heightMap, uv).rgb * terrainColor;
}
