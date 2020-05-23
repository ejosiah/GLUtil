// HLSL compatability

#define uint1 uint
#define uint2 uvec2
#define uint3 uvec3
#define uint4 uvec4

#define int1 int
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4

#define float1 float
#define float2 vec2
#define float3 vec3
#define float4 vec4

#define bool1 bool
#define bool2 bvec2
#define bool3 bvec3
#define bool4 bvec4

#define half float
#define half1 float
#define half2 vec2
#define half3 vec3
#define half4 vec4

#define rsqrt inversesqrt

#define tex2D texture2D

#define lerp mix

#define ddx dFdx
#define ddy dFdy

float frac(float x) {
    return fract(x);
}

float2 frac(float2 x) {
    return fract(x);
}

float3 frac(float3 x) {
    return fract(x);
}

float4 frac(float4 x) {
    return fract(x);
}

float atan2(float y, float x) {
    return atan(y, x);
}

float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

float2 saturate(float2 x) {
    return clamp(x, float2(0.0), float2(1.0));
}

float3 saturate(float3 x) {
    return clamp(x, float3(0.0), float3(1.0));
}

float4 saturate(float4 x) {
    return clamp(x, float4(0.0), float4(1.0));
}
