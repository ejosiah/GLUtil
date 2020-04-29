#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10

layout(binding = 0) uniform sampler2D albedoMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D metalnessMap;
layout(binding = 3) uniform sampler2D roughnessMap;
layout(binding = 4) uniform sampler2D aoMap;
layout(binding = 5) uniform samplerCube irradianceMap;
layout(binding = 6) uniform samplerCube prefilterMap;
layout(binding = 7) uniform sampler2D brdfLUT;

const int Power = 0;
const int Intensity = 1; // power/sr
const int Illuminance = 2; // power /area
const int Luminance = 3; // power /(sr * area)

const int Point = 0;
const int Spot = 1;
const int Direct = 2;
const int Sun = 3;
const int AreaSphere = 4;
const int AreaDisk = 5;
const int AreaRectangle = 6;

struct Light{
	//int unit;
	// int type;
	vec3 position;
	vec3 intensity;
//	vec3 color;
};

struct Material{
	vec3 albedo;
	float metalness;
	float roughness;
	vec3 ao;
};

layout(std430, binding=0) buffer SCENE_SSBO{
	vec3 eyes;
	Light lights[MAX_SCENE_LIGHTS];
};
  
uniform int numLights = 6;

in VERTEX{
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_in;

out vec4 fragColor;

uniform Material material;
uniform bool glossiness = true;
uniform bool directional = true;
uniform bool invertBlack = false;
uniform bool useNormalMapping = false;
uniform bool useTexture = true;
uniform bool ibl = false;

float saturate(float x){
	return max(x, 0);
}

float DistributionGGX(vec3 N, vec3 H, float a){
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k){
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k){
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

vec3 getNormal(){
	if(!useNormalMapping) return normalize(vertex_in.normal);
	vec3 n = normalize(vertex_in.normal);
	vec3 t = normalize(vertex_in.tangent);
	vec3 b = normalize(vertex_in.bitangent);
	vec3 tangentNormal = texture(normalMap, vertex_in.uv).xyz * 2 - 1;

	mat3 olm_inv = inverse(mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z));
	vec3 tNormal = normalize((2.0 * texture(normalMap, vertex_in.uv) - 1.0).xyz);
	return olm_inv * tNormal;
}

vec3 getAlbedo(){
	vec3 albedo = useTexture ? texture(albedoMap, vertex_in.uv).rgb : material.albedo;
	albedo = invertBlack && all(equal(albedo, vec3(0))) ? vec3(1) : albedo;
	return albedo;
}

float getMetalness(){
	return useTexture ? texture(metalnessMap, vertex_in.uv).r : material.metalness;
}

float getRoughness(){
	float roughness = useTexture ? texture(roughnessMap, vertex_in.uv).r : material.roughness;
	roughness = glossiness ? pow((1 - roughness), 4) : roughness;
	return roughness;
}

vec3 getAo(){
	return useTexture ? texture(aoMap, vertex_in.uv).rgb : material.ao;
}


const vec3 dielectric = vec3(0.04);
const float MAX_REFLECTION_LOD = 4.0;

void main(){
	vec3 worldPos = vertex_in.position;
	vec2 uv = vertex_in.uv;
	vec3 albedo = getAlbedo();

	float metalness = getMetalness();
	vec3 ao = getAo();
	float roughness = getRoughness();
	
	vec3 N = getNormal();
	vec3 wo =  normalize(eyes - worldPos);

	vec3 F0 = mix(dielectric, albedo, metalness);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < numLights; i++){
		Light l = lights[i];
		vec3 lightDir = directional ? l.position : (l.position - worldPos);
		vec3 wi = normalize(lightDir);
		vec3 H = normalize(wi + wo);

		float d = max(directional ? 1.0 : length(lightDir), 0.01);
		float attenuation = 1.0 / (d * d);
		vec3 radiance = l.intensity * attenuation;

		float NDF = DistributionGGX(N, H, roughness);

		float k = pow(roughness + 1, 2)/8;

		float G = GeometrySmith(N, wo, wi, k);

		vec3 F  = fresnelSchlick(max(dot(H, wo), 0.0), F0);

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd *= 1.0 - metalness;

		vec3 num = NDF * G * F;
		float denom = 4.0 * max(dot(N, wo), 0.0) * max(dot(N, wi), 0.0);
		vec3 specular = num/max(denom, 0.001);

		Lo += (kd * albedo / PI + specular) * radiance * max(dot(N, wi), 0.0);
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec4 c;
	if(ibl){
		vec3 F = fresnelSchlickRoughness(max(dot(N, wo), 0.0), F0, roughness);
		vec3 Ks = F;
		vec3 Kd = 1 - Ks;
		Kd *= 1.0 - metalness;
		vec3 irradiance = texture(irradianceMap, N).rgb;
		vec3 diffuse = irradiance * albedo;

		vec3 R = reflect(-wo, N);
		vec3 prefilterColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
		vec2 brdf = texture(brdfLUT, vec2(max(dot(N, wo), 0), roughness)).rg;
		vec3 specular = prefilterColor * (F * brdf.x + brdf.y);

		ambient = (Kd * diffuse + specular) * ao;
	}

	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    fragColor = vec4(color, 1.0);
}