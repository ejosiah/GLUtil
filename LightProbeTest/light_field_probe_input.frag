#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10


layout(binding = 0) uniform sampler2D ambientMap;
layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 3) uniform sampler2D normalMap;


in VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 uv;
} vert_in;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform float shininess = 5.0;
uniform vec3 emission = vec3(0.0);

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec4 o_distance;

const vec3 globalAmbience = vec3(0.3);
const vec3 lightColor = vec3(0.3);
vec3 worldPos = vert_in.position;
vec2 uv = vert_in.uv;
vec4 posInLight = vert_in.lightSpacePos;

vec3 packNormal(vec3 N)
{
	return N * vec3(0.5) + vec3(0.5);
}

vec3 unpackNormal(vec3 packedNormal)
{
	return normalize(packedNormal * vec3(2.0) - vec3(1.0));
}

vec3 getNormal() {
	vec3 n = normalize(vert_in.normal);
	vec3 t = normalize(vert_in.tangent);
	vec3 b = normalize(vert_in.bitangent);

	mat3 olm_inv = inverse(mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z));
	vec3 tNormal = 2.0 * texture(normalMap, vert_in.uv).xyz - 1.0;
	vec3 normal = olm_inv * tNormal;
	return gl_FrontFacing ? normal : -normal;
}

vec3 getNormal0() {
	vec3 Q1 = dFdx(worldPos);
	vec3 Q2 = dFdy(worldPos);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N = normalize(vert_in.normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	vec3 tNormal = 2.0 * texture(normalMap, vert_in.uv).xyz - 1.0;
	//return normalize(TBN * tNormal);
	return gl_FrontFacing ? N : -N;
	//return N;
}

float ShadowCalculation(vec3 worldPos, vec4 posInLight, vec3 lightPos, vec3 camPos, float NdotL);


void main() {
	vec3 viewDir = camPos - worldPos;
	vec3 L = normalize(lightPos - worldPos);
	vec3 V = normalize(viewDir);
	vec3 N = getNormal0();
	vec3 H = normalize(V + L);

	vec3 ambient = texture(ambientMap, uv).rgb;

	vec3 specular = texture(specularMap, uv).rgb * max(pow(dot(N, H), shininess), 0);

	float NdotL = max(dot(N, L), 0);
	vec3 diffuse = texture(diffuseMap, uv).rgb * NdotL;

	float shadow = ShadowCalculation(worldPos, posInLight, lightPos, camPos, NdotL);

	vec3 color = globalAmbience * ambient + (1 - shadow) * lightColor * (diffuse + specular);

	o_normal = vec4(packNormal(N), 0);
	o_distance = vec4(length(viewDir), dot(viewDir, viewDir), 0, 0);
	
	fragColor = vec4(color, 1);
}

#pragma include("shadow.glsl")