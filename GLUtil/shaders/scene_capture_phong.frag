#version 450 core
#pragma debug(on)
#pragma optimize(off)

#pragma include("constants.glsl")


layout(binding = 0) uniform sampler2D ambientMap;
layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 3) uniform sampler2D normalMap;

#pragma include("LightFieldProbe.glsl")

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
uniform bool lfp_on = false;
uniform bool isGround = false;

layout(location = 0) out vec4 fragColor;

const vec3 globalAmbience = vec3(0.3);
uniform vec3 lightColor = vec3(0.3);
vec3 worldPos = vert_in.position;
vec2 uv = vert_in.uv;
vec4 posInLight = vert_in.lightSpacePos;

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
	//return gl_FrontFacing ? N : -N;
	return N;
}

float ShadowCalculation(vec3 worldPos, vec4 posInLight, vec3 lightPos, vec3 camPos, float NdotL);


void main() {
	vec3 L = normalize(lightPos - worldPos);
	vec3 V = normalize(camPos - worldPos);
	vec3 N = getNormal0();
	vec3 H = normalize(V + L);

	vec3 ambient = texture(ambientMap, uv).rgb;

	vec3 specular = texture(specularMap, uv).rgb * max(pow(dot(N, H), shininess), 0);

	float NdotL = max(dot(N, L), 0);
	vec3 diffuse = texture(diffuseMap, uv).rgb * NdotL;

	vec3 diffuse_indirect = vec3(0);
	if(lfp_on){
		diffuse_indirect += computePrefilteredIrradiance(worldPos, N) * INV_TWO_PI;
		specular += computeGlossyRay(worldPos, V, N);
		diffuse += diffuse_indirect;
	}

	float shadow = ShadowCalculation(worldPos, posInLight, lightPos, camPos, NdotL);

	vec3 color = globalAmbience * lightColor * ambient + (1 - shadow) * lightColor * (diffuse + specular);

	fragColor = vec4(color, vert_in.color.a);
	//fragColor = vec4(diffuse_indirect, vert_in.color.a);
}

#pragma include("shadow.glsl")