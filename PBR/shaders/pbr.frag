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

struct Light{
	vec3 position;
	vec3 intensity;
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
	smooth vec3 wo;
	smooth vec3 wi[MAX_SCENE_LIGHTS];
} vertex_in;

out vec4 fragColor;

//uniform vec3 albedo;
//uniform float metalness;
//uniform float roughness;
//uniform vec3 ao = vec3(1);
uniform bool glossiness = true;
uniform bool directional = true;
uniform bool invertBlack = false;
uniform bool useNormalMapping = false;

float DistributionGGX(vec3 N, vec3 H, float a);

float GeometrySchlickGGX(float NdotV, float k);

float GeometrySmith(vec3 N, vec3 V, vec3 L, float a);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

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



const vec3 dielectric = vec3(0.04);

void main(){
	vec3 worldPos = vertex_in.position;
	vec2 uv = vertex_in.uv;
	vec3 albedo = texture(albedoMap, uv).rgb;
	albedo = invertBlack && all(equal(albedo, vec3(0))) ? vec3(1) : albedo;

	float metalness = texture(metalnessMap, uv).r;
	vec3 ao = texture(aoMap, uv).rgb;
	float roughness = texture(roughnessMap, uv).r;
	roughness = glossiness ? 1 - roughness : roughness;
	
	vec3 N = getNormal();
	vec3 wo =  normalize(eyes - worldPos);

	vec3 F0 = mix(dielectric, albedo, metalness);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < numLights; i++){
		Light l = lights[i];
		vec3 lightDir = directional ? l.position : (l.position - worldPos);
		vec3 wi = normalize(lightDir);
		vec3 H = normalize(wi + wo);

		float d = directional ? 1.0 : length(lightDir);
		float attenuation = 1.0 / (d * d);
		vec3 radiance = l.intensity * attenuation;

		float NDF = DistributionGGX(N, H, roughness);

		float k = pow(roughness, 2)/8;

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
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    fragColor = vec4(color, 1.0);
	//fragColor = texture(aoMap, uv);
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
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}