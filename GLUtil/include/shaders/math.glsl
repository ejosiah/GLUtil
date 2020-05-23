#pragma include("constants.glsl")
#pragma include("compatibility.glsl")

const float pi = 3.1415927;

/** 1 / pi */
const float invPi = 1.0 / pi;
const float inv8Pi = 1.0 / (8.0 * pi);

const float meters = 1.0;
const float centimeters = 0.01;
const float millimeters = 0.001;

/** 32-bit floating-point infinity */
const float inf =
#if __VERSION__ >= 420
// Avoid the 1/0 underflow warning (requires GLSL 400 or later):
intBitsToFloat(0x7f800000);
#else
1.0 / 0.0;
#endif

#ifndef vec1
#define vec1 float
#endif

#define Vector2 vec2
#define Point2  vec2
#define Vector3 vec3
#define Point3  vec3
#define Vector4 vec4

#define Color3  vec3
#define Radiance3 vec3
#define Biradiance3 vec3
#define Irradiance3 vec3
#define Radiosity3 vec3
#define Power3 vec3

#define Color4  vec4
#define Radiance4 vec4
#define Biradiance4 vec4
#define Irradiance4 vec4
#define Radiosity4 vec4
#define Power4 vec4

#define Vector2int32 int2
#define Vector3int32 int3
#define Matrix4      mat4
#define Matrix3      mat3
#define Matrix2      mat2

#define CFrame       mat4x3

float square(float x) {
    return x * x;
}

float max3(float a, float b, float c) {
    return max(a, max(b, c));
}

float max4(float a, float b, float c, float d) {
    return max(max(a, d), max(b, c));
}

float min3(float a, float b, float c) {
    return min(a, min(b, c));
}

float min4(float a, float b, float c, float d) {
    return min(min(a, d), min(b, c));
}

float maxComponent(vec2 a) {
    return max(a.x, a.y);
}

float maxComponent(vec3 a) {
    return max3(a.x, a.y, a.z);
}

float maxComponent(vec4 a) {
    return max4(a.x, a.y, a.z, a.w);
}

float Saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

float lengthSquared(vec2 v) {
    return dot(v, v);
}

float lengthSquared(vec3 v) {
    return dot(v, v);
}

float lengthSquared(vec4 v) {
    return dot(v, v);
}

vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float roughness) {
    float lerpFactor = (1 - roughness);
    return normalize(mix(N, R, lerpFactor));
}

float DistributionGGX(vec3 N, vec3 H, float a) {
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k) {
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k) {
    float NdotV = Saturate(dot(N, V));
    float NdotL = Saturate(dot(N, L));
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}