#version 450 core

layout(binding = 0) uniform sampler2D heightMap;

uniform bool generated;
uniform vec3 camPos;
uniform vec3 sunPos;

in ncl_PerVertex{
	vec3 worldPos;
	vec3 normal;
	vec2 uv;
};

const vec3 terrainColor = vec3(0.5, 0.25, 0.02);
const vec3 Intensity = vec3(1);

out vec4 fragColor;


void main(){
	vec3 L = normalize(sunPos - worldPos);
	vec3 V = normalize(camPos - worldPos);
	vec3 N = normalize(normal);
	vec3 E = normalize(L + V);

	vec3 ambient = 0.3 * terrainColor;
	vec3 diffuse = max(0, dot(L, N)) * terrainColor;
	vec3 specular =pow(max(0, dot(E, N)), 50) * terrainColor;

	fragColor.a = 1;
	fragColor.rgb = Intensity * (ambient + diffuse + specular);
}
