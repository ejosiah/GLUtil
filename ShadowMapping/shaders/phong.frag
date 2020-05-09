#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define MAX_SCENE_LIGHTS 10

uniform sampler2D ambientMap;
layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D shadowMap;
layout(binding = 5) uniform samplerCube pointShadowMap;

const int DIRECTIONAL_SHADOW = 0;
const int POINT_SHADOW = 1;

in VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_in;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform float shininess = 5.0;
uniform vec3 emission = vec3(0.0);
uniform bool shadowOn = false;
uniform bool pointLight = true;
uniform float farPlane;
uniform int shadowType;

out vec4 fragColor;

const vec3 globalAmbience = vec3(0.3);
const vec3 lightColor = vec3(0.3);
vec3 worldPos = vertex_in.position;
vec2 uv = vertex_in.uv;
vec4 posInLight = vertex_in.lightSpacePos;

vec3 getNormal(){
	vec3 n = normalize(vertex_in.normal);
	vec3 t = normalize(vertex_in.tangent);
	vec3 b = normalize(vertex_in.bitangent);

	mat3 olm_inv = inverse(mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z));
	vec3 tNormal = 2.0 * texture(normalMap, vertex_in.uv).xyz - 1.0;
	vec3 normal =  olm_inv * tNormal;
	return gl_FrontFacing ? normal : -normal;
}

vec3 getNormal0(){
	vec3 Q1 = dFdx(worldPos);
	vec3 Q2 = dFdy(worldPos);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N   = normalize(vertex_in.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

	vec3 tNormal = 2.0 * texture(normalMap, vertex_in.uv).xyz - 1.0;
	//return normalize(TBN * tNormal);
	//return gl_FrontFacing ? N : -N;
	return N;
}

float ShadowCalculationDirectional(vec4 pos, float NdotL){
	if(!shadowOn) return 0.0;

    vec3 projCoords = pos.xyz / pos.w;
    projCoords = projCoords * 0.5 + 0.5; // [-1, 1] to [0, 1]

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - NdotL), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float ShadowCalculationPoint(vec3 pos, float NdotL){
	if(!shadowOn) return 0.0;
	vec3 fragToLight = pos - lightPos;

	float closestDepth = texture(pointShadowMap, fragToLight).r;

	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = max(0.05 * (1.0 - NdotL), 0.005);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;


	fragColor = vec4(vec3(closestDepth/farPlane), 1.0);
	return shadow;
}

float ShadowCalculation(vec3 posInWorld, vec4 posInLight, float NdotL){
	if(!shadowOn) return 0.0;
	if(shadowType == DIRECTIONAL_SHADOW) return ShadowCalculationDirectional(posInLight, NdotL);
	if(shadowType == POINT_SHADOW) return ShadowCalculationPoint(posInWorld, NdotL);
	return 0.0;
}



void main(){
	vec3 L = normalize(lightPos - worldPos);
	vec3 V = normalize(camPos - worldPos);
	vec3 N = getNormal0();
	vec3 H = normalize(V + L);

	vec3 ambient = texture(ambientMap, uv).rgb; 

	vec3 specular = texture(specularMap, uv).rgb * max(pow(dot(N, H), shininess), 0);

	float NdotL = max(dot(N, L), 0);
	vec3 diffuse = texture(diffuseMap, uv).rgb  * NdotL;

	float shadow = ShadowCalculation(worldPos, posInLight, NdotL);

	vec3 color =  globalAmbience * ambient + (1 - shadow) *  lightColor * (diffuse + specular);

//	color = color / (color + vec3(1.0));
//    color = pow(color, vec3(1.0/2.2)); 

	fragColor = vec4(color, vertex_in.color.a);

}