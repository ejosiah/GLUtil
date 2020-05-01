#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10


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
	int unit;
	int type;
	float value;
	vec3 position;
	vec3 color;
	float radius;
	vec3 normal;
};

in VERTEX{
	smooth vec3 position;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_in;

uniform Light light;
uniform vec3 eyes;
uniform float roughness = 0.1;
uniform vec3 lightNormal;
out vec4 fragColor;


float Saturate(float x){
	return max(x, 0.0);
}

float illuminanceSphereOrDisk(float cosTheta, float sinSigmaSqr){
	
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	float illuminance = 0.0f;
	if(cosTheta * cosTheta > sinSigmaSqr){
		illuminance = PI * sinSigmaSqr * Saturate(cosTheta);
	}else{
		float x = sqrt(1.0 / sinSigmaSqr - 1.0f);
		float y = -x * (cosTheta / sinTheta);
		float sinThetaSqrtY = sinTheta * sqrt(1.0f - y * y);
		illuminance = ( cosTheta * acos (y) - x * sinThetaSqrtY ) * sinSigmaSqr + atan (sinThetaSqrtY / x);
	}

	return max(illuminance, 0.0);
}

vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float roughness){
	float lerpFactor = (1 - roughness);
	return normalize(mix(R, N, lerpFactor));
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
    float NdotV = Saturate(dot(N, V));
    float NdotL = Saturate(dot(N, L));
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

struct LightPart{
	vec3 diffuse;
	vec3 specular;
};

struct Lighting{
	LightPart direct;
	LightPart indirect;
};

struct Surface{
	vec3 position;
	vec3 normal;
};

void SphereLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = light.position;

	vec3 lightDir = lightPos - worldPos;

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	vec3 r = reflect(-V, N);
	vec3 H = normalize(L+V);

	float NdotL = dot(N, L);
//	vec3 color = vertex_in.color.xyz * light.color * max(NdotL, 0);
	vec3 color = vertex_in.color.xyz;

	float sqrDist = dot(lightPos, lightPos);
	float cosTheta = clamp(NdotL, -0.999, 0.999);
	float sqrLightRadius = pow(light.radius, 2);
	float sinSigmaSqr = min(sqrLightRadius / sqrDist, 0.9999f);
	float illuminance = illuminanceSphereOrDisk(cosTheta, sinSigmaSqr);
	float intensity = light.value/( 4 * sqrLightRadius  * PI * PI);
	vec3 radiance = light.color * intensity * illuminance;


	r = getSpecularDominantDirArea(N, r, roughness);
	vec3 c = dot(L, r) * r - L;
	vec3 cp = L + c * Saturate(light.radius/length(c));
	L = normalize(cp);

	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	lighting.direct.diffuse = max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular = max(vec3(0), specular * radiance * Saturate(NdotL));
}

float tracePlane(vec3 o, vec3 d, vec3 po, vec3 pn){
	return dot(pn, (po - o) / dot(pn, d));
}

void DiskLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = light.position;

	vec3 lightDir = lightPos - worldPos;

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	vec3 r = reflect(-V, N);
	vec3 H = normalize(L+V);
	vec3 Na = normalize(light.normal);

	float sqrDist = dot(lightDir, lightDir);
	float NdotL = dot(N, L);
	float cosTheta = NdotL;
	float sqrLightRadius = 0.5 * 0.5;
	float sinSigmaSqr = sqrLightRadius / (sqrLightRadius + max(sqrLightRadius, sqrDist));

	float illuminance = illuminanceSphereOrDisk(cosTheta, sinSigmaSqr) * Saturate(dot(Na, -L));
	float intensity = light.value/( sqrLightRadius  * PI * PI);
	vec3 radiance = light.color * intensity * illuminance;

	r = getSpecularDominantDirArea(N, r, roughness);

	float specularAttenuation = Saturate(abs(dot(Na, r)));

	if(specularAttenuation > 0){
		float t = tracePlane(worldPos, r, lightPos, Na);
		vec3 p = worldPos + r * t;
		vec3 centerToRay = p - lightPos;
		vec3 closetPoint = L + centerToRay * Saturate(0.5 / length(centerToRay));
		L = normalize(closetPoint);
	}

	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	vec3 color = vertex_in.color.xyz;
	lighting.direct.diffuse = max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular = max(vec3(0), specularAttenuation * specular * radiance * Saturate(NdotL));

}


void main(){

	vec3 lightPos = light.position;
	vec3 lightDir = lightPos - vertex_in.position;

	Surface surface;
	surface.position = vertex_in.position;
	surface.normal = vertex_in.normal;

	Lighting lighting;
//	SphereLight(light, surface, eyes, lighting);
	DiskLight(light, surface, eyes, lighting);
	vec3 color = lighting.direct.diffuse + lighting.direct.specular;

	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

	fragColor = vec4(color, 1);
}