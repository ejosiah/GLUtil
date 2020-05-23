#pragma include("compatibility.glsl")


const float pi = 3.1415927;

const float invPi = 1.0 / pi;
const float inv8Pi = 1.0 / (8.0 * pi);

const float meters = 1.0;
const float centimeters = 0.01;
const float millimeters = 0.001;

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

Matrix3 referenceFrameFromZAxis(Vector3 z) {
    Vector3 y = (abs(z.y) > 0.85) ? Vector3(-1, 0, 0) : Vector3(0, 1, 0);
    Vector3 x = normalize(cross(y, z));
    y = normalize(cross(z, x));
    return Matrix3(x, y, z);
}

Matrix3 _referenceFrameFromZAxis(Vector3 n) {
    float s = n.z >= 0.0 ? 1.0 : -1.0;
    float a = -1.0 / (s + n.z);
    float b = n.x * n.y * a;
    Vector3 X = Vector3(1.0 + s * n.x * n.x * a, s * b, -s * n.x);
    Vector3 Y = Vector3(b, s + n.y * n.y * a, -n.y);
    return Matrix3(X, Y, n);
}


#foreach(gentype) in (int), (ivec2), (ivec3), (ivec4), (float), (vec2), (vec3), (vec4)


void swap(inout $(gentype) a, inout $(gentype) b) {
    $(gentype) temp = a;
    a = b;
    b = temp;
}

$(gentype) square($(gentype) x) {
    return x * x;
}

$(gentype) pow2($(gentype) x) {
    return x * x;
}

$(gentype) pow3($(gentype) x) { return x * square(x); }

/** Compute x^4 */
$(gentype) pow4($(gentype) x) {
    x = x * x;
    return x * x;
}

/** Computes x<sup>5</sup> */
$(gentype) pow5($(gentype) x) { return x * pow4(x); }


/** Compute x^6 */
$(gentype) pow6($(gentype) x) {
    // x^3
    x *= x * x;

    // x^6;
    return x * x;
}

$(gentype) pow7($(gentype) x) { return x * pow6(x); }

/** Compute x^8 */
$(gentype) pow8($(gentype) x) {
    // x^2
    x *= x;

    // x^4
    x *= x;

    // x^8;
    return x * x;
}

/** Compute x^8 */
$(gentype) pow16($(gentype) x) {
    // x^2
    x *= x;

    // x^4
    x *= x;

    // x^8;
    x *= x;

    // x^16
    return x * x;
}

/** Compute v^64 */
$(gentype) pow64($(gentype) v) {
    // v^2
    v *= v;

    // v^4
    v *= v;

    // v^64
    return v * v * v;
}
#endforeach


#foreach(gentype) in (float), (vec2), (vec3), (vec4)

    $(gentype) smootherstep($(gentype) edge0, $(gentype) edge1, $(gentype) x) {
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

/** Avoids the clamping and division for smootherstep(0, 1, x) when x is known to be on the range [0, 1] */
$(gentype) unitSmootherstep($(gentype) x) {
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

/** Avoids the clamping and division for smoothstep(0, 1, x) when x is known to be on the range [0, 1] */
$(gentype) unitSmoothstep($(gentype) x) {
    return x * x * (3.0 - 2.0 * x);
}
#endforeach

#foreach(gentype) in (vec2), (vec3), (vec4)
$(gentype) smootherstep(float edge0, float edge1, $(gentype) x) {
    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}
#endforeach


#foreach(vectype, gentype) in (vec2, float), (vec3, float), (vec4, float)
$(gentype) lengthSquared($(vectype) v) {
    return dot(v, v);
}
#endforeach


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

float sum(float a) { return a; }
float sum(float2 a) { return a.x + a.y; }
float sum(float3 a) { return a.x + a.y + a.z; }
float sum(float4 a) { return a.x + a.y + a.z + a.w; }

float mean(float a) { return a; }
float mean(float2 a) { return dot(a, float2(0.5)); }
float mean(float3 a) { return dot(a, float3(1.0 / 3.0)); }
float mean(float4 a) { return dot(a, float4(0.25)); }

float minComponent(vec2 a) {
    return min(a.x, a.y);
}

float minComponent(vec3 a) {
    return min3(a.x, a.y, a.z);
}

float minComponent(vec4 a) {
    return min4(a.x, a.y, a.z, a.w);
}

float meanComponent(vec4 a) {
    return (a.r + a.g + a.b + a.a) * (1.0 / 4.0);
}

float meanComponent(vec3 a) {
    return (a.r + a.g + a.b) * (1.0 / 3.0);
}

float meanComponent(vec2 a) {
    return (a.r + a.g) * (1.0 / 2.0);
}

float meanComponent(float a) {
    return a;
}


vec3 schlickFresnel(in vec3 F0, in float cos_i, float smoothness) {
    return (F0.r + F0.g + F0.b > 0.0) ? mix(F0, vec3(1.0), 0.9 * pow5(square(smoothness) * (1.0 - max(0.0, cos_i)))) : F0;
}


float smoothnessToBlinnPhongExponent(in float g3dSmoothness) {
    // From Graphics Codex [smthnss]
    float academicRoughness = square(1.0 - g3dSmoothness);

    // From http://simonstechblog.blogspot.com/2011/12/microfacet-brdf.html
    return max(0.0, 2.0 / square(academicRoughness) - 2.0);
}

float packGlossyExponent(in float blinnPhongExponent) {
    // From http://simonstechblog.blogspot.com/2011/12/microfacet-brdf.html
    float academicRoughness = sqrt(2.0 / (blinnPhongExponent + 2.0));

    // From Graphics Codex [smthnss]
    float g3dSmoothness = 1 - sqrt(academicRoughness);

    // Never let the exponent go above the max representable non-mirror value in a uint8
    return min(g3dSmoothness, 254.0 / 255.0);

    // return (clamp(sqrt((x - 0.5f) * (1.0f / 8192.0f)), 0.0, 1.0) * 253.0 + 1.0) * (1.0 / 255.0);
}


#if G3D_SHADER_STAGE == G3D_FRAGMENT_SHADER
float computeSampleRate(vec2 texCoord, vec2 samplerSize) {
    texCoord *= samplerSize;
    return maxComponent(max(abs(dFdx(texCoord)), abs(dFdy(texCoord))));
}
#else
float computeSampleRate(vec2 texCoord, vec2 samplerSize) {
    return 0.0;
}
#endif


/** http://blog.selfshadow.com/2011/07/22/specular-showdown/ */
float computeToksvigGlossyExponent(float rawGlossyExponent, float rawNormalLength) {
    rawNormalLength = min(1.0, rawNormalLength);
    float ft = rawNormalLength / lerp(max(0.1, rawGlossyExponent), 1.0, rawNormalLength);
    float scale = (1.0 + ft * rawGlossyExponent) / (1.0 + rawGlossyExponent);
    return scale * rawGlossyExponent;
}

/** CIE luminance (Y) of linearRGB value (not sRGB!) */
float RGBtoCIELuminance(vec3 linearRGB) {
    return dot(vec3(0.2126, 0.7152, 0.0722), linearRGB);
}

/** \cite http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl */
vec3 RGBtoHSV(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = (c.g < c.b) ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = (c.r < p.x) ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float eps = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + eps)), d / (q.x + eps), q.x);
}

/** \cite http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl */
vec3 HSVtoRGB(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), clamp(c.y, 0.0, 1.0));
}

vec2 hammersleySquare(uint i, const uint N) {
    vec2 P;
    P.x = float(i) * (1.0 / float(N));

    i = (i << 16u) | (i >> 16u);
    i = ((i & 0x55555555u) << 1u) | ((i & 0xAAAAAAAAu) >> 1u);
    i = ((i & 0x33333333u) << 2u) | ((i & 0xCCCCCCCCu) >> 2u);
    i = ((i & 0x0F0F0F0Fu) << 4u) | ((i & 0xF0F0F0F0u) >> 4u);
    i = ((i & 0x00FF00FFu) << 8u) | ((i & 0xFF00FF00u) >> 8u);
    P.y = float(i) * 2.3283064365386963e-10; // / 0x100000000

    return P;
}

vec3 hammersleyHemi(uint i, const uint N) {
    vec2 P = hammersleySquare(i, N);
    float phi = P.y * 2.0 * pi;
    float cosTheta = 1.0 - P.x;
    float sinTheta = sqrt(1.0 - square(cosTheta));
    return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}


vec3 hammersleyCosHemi(uint i, const uint N) {
    vec3 P = hammersleyHemi(i, N);
    P.z = sqrt(P.z);
    return P;
}

mat4x4 identity4x4() {
    return mat4x4(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

/** Constructs a 4x4 translation matrix, assuming T * v multiplication. */
mat4x4 translate4x4(Vector3 t) {
    return mat4x4(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t, 1);
}

/** Constructs a 4x4 Y->Z rotation matrix, assuming R * v multiplication. a is in radians.*/
mat4x4 pitch4x4(float a) {
    return mat4x4(1, 0, 0, 0,
        0, cos(a), -sin(a), 0,
        0, sin(a), cos(a), 0,
        0, 0, 0, 1);
}

/** Constructs a 4x4 X->Y rotation matrix, assuming R * v multiplication. a is in radians.*/
mat4x4 roll4x4(float a) {
    return mat4x4(cos(a), -sin(a), 0, 0,
        sin(a), cos(a), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

/** Constructs a 4x4 Z->X rotation matrix, assuming R * v multiplication. a is in radians.*/
mat4x4 yaw4x4(float a) {
    return mat4x4(cos(a), 0, sin(a), 0,
        0, 1, 0, 0,
        -sin(a), 0, cos(a), 0,
        0, 0, 0, 1);
}

#if G3D_SHADER_STAGE == G3D_FRAGMENT_SHADER
float mipMapLevel(in vec2 texture_coordinate, in vec2 textureSize) {
    vec2  dx = dFdx(texture_coordinate);
    vec2  dy = dFdy(texture_coordinate);
    float delta_max_sqr = max(dot(dx, dx), dot(dy, dy));

    return 0.5 * log2(delta_max_sqr * square(maxComponent(textureSize))); // == log2(sqrt(delta_max_sqr));
}
#endif

vec4 textureLodSmooth(sampler2D tex, vec2 uv, int lod) {
    vec2 res = textureSize(tex, lod);
    uv = uv * res + 0.5;
    vec2 iuv = floor(uv);
    vec2 fuv = fract(uv);
    uv = iuv + fuv * fuv * (3.0 - 2.0 * fuv); // fuv*fuv*fuv*(fuv*(fuv*6.0-15.0)+10.0);;
    uv = (uv - 0.5) / res;
    return textureLod(tex, uv, lod);
}


vec4 textureLodBicubic(sampler2D tex, vec2 uv, int lod) {
    //--------------------------------------------------------------------------------------
    // Calculate the center of the texel to avoid any filtering

    float2 textureDimensions = textureSize(tex, lod);
    float2 invTextureDimensions = 1.0 / textureDimensions;

    uv *= textureDimensions;

    float2 texelCenter = floor(uv - 0.5) + 0.5;
    float2 fracOffset = uv - texelCenter;
    float2 fracOffset_x2 = fracOffset * fracOffset;
    float2 fracOffset_x3 = fracOffset * fracOffset_x2;

    //--------------------------------------------------------------------------------------
    // Calculate the filter weights (B-Spline Weighting Function)

    float2 weight0 = fracOffset_x2 - 0.5 * (fracOffset_x3 + fracOffset);
    float2 weight1 = 1.5 * fracOffset_x3 - 2.5 * fracOffset_x2 + 1.0;
    float2 weight3 = 0.5 * (fracOffset_x3 - fracOffset_x2);
    float2 weight2 = 1.0 - weight0 - weight1 - weight3;

    //--------------------------------------------------------------------------------------
    // Calculate the texture coordinates

    float2 scalingFactor0 = weight0 + weight1;
    float2 scalingFactor1 = weight2 + weight3;

    float2 f0 = weight1 / (weight0 + weight1);
    float2 f1 = weight3 / (weight2 + weight3);

    float2 texCoord0 = texelCenter - 1.0 + f0;
    float2 texCoord1 = texelCenter + 1.0 + f1;

    texCoord0 *= invTextureDimensions;
    texCoord1 *= invTextureDimensions;

    //--------------------------------------------------------------------------------------
    // Sample the texture

    return textureLod(tex, float2(texCoord0.x, texCoord0.y), lod) * scalingFactor0.x * scalingFactor0.y +
        textureLod(tex, float2(texCoord1.x, texCoord0.y), lod) * scalingFactor1.x * scalingFactor0.y +
        textureLod(tex, float2(texCoord0.x, texCoord1.y), lod) * scalingFactor0.x * scalingFactor1.y +
        textureLod(tex, float2(texCoord1.x, texCoord1.y), lod) * scalingFactor1.x * scalingFactor1.y;
}

/* L1 norm */
float norm1(vec3 v) {
    return abs(v.x) + abs(v.y) + abs(v.z);
}


float norm2(vec3 v) {
    return length(v);
}


/** L^4 norm */
float norm4(vec2 v) {
    v *= v;
    v *= v;
    return pow(v.x + v.y, 0.25);
}

/** L^8 norm */
float norm8(vec2 v) {
    v *= v;
    v *= v;
    v *= v;
    return pow(v.x + v.y, 0.125);
}

float normInf(vec3 v) {
    return maxComponent(abs(v));
}

float infIfNegative(float x) { return (x >= 0.0) ? x : inf; }


struct Ray {
    Point3      origin;
    /** Unit direction of propagation */
    Vector3     direction;
};


/** */
uint extractEvenBits(uint x) {
    x = x & 0x55555555u;
    x = (x | (x >> 1)) & 0x33333333u;
    x = (x | (x >> 2)) & 0x0F0F0F0Fu;
    x = (x | (x >> 4)) & 0x00FF00FFu;
    x = (x | (x >> 8)) & 0x0000FFFFu;

    return x;
}


uint expandBits(uint v) {
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;

    return v;
}


uint interleaveBits(uvec3 inputval) {
    uint xx = expandBits(inputval.x);
    uint yy = expandBits(inputval.y);
    uint zz = expandBits(inputval.z);

    return xx + yy * uint(2) + zz * uint(4);
}


float signNotZero(in float k) {
    return (k >= 0.0) ? 1.0 : -1.0;
}


vec2 signNotZero(in vec2 v) {
    return vec2(signNotZero(v.x), signNotZero(v.y));
}


bool isFinite(float x) {
    return !isnan(x) && (abs(x) != inf);
}


Vector3 sphericalFibonacci(float i, float n) {
    const float PHI = sqrt(5) * 0.5 + 0.5;
#   define madfrac(A, B) ((A)*(B)-floor((A)*(B)))
    float phi = 2.0 * pi * madfrac(i, PHI - 1);
    float cosTheta = 1.0 - (2.0 * i + 1.0) * (1.0 / n);
    float sinTheta = sqrt(saturate(1.0 - cosTheta * cosTheta));

    return Vector3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta);

#   undef madfrac
}

Vector3 sphereRandom(vec2 r) {
    float cosPhi = r.x * 2.0 - 1.0;
    float sinPhi = sqrt(1 - square(cosPhi));
    float theta = r.y * 2.0 * pi;
    return Vector3(sinPhi * cos(theta), sinPhi * cos(theta), cosPhi);
}

Vector3 hemisphereRandom(vec2 r) {
    Vector3 s = sphereRandom(r);
    return Vector3(s.x, s.y, abs(s.z));
}

Vector3 cosPowHemiRandom(vec2 r, const float k) {
    float cos_theta = pow(r.x, 1.0 / (k + 1.0));
    float sin_theta = sqrt(1.0f - square(cos_theta));
    float phi = 2 * pi * r.y;

    return Vector3(cos(phi) * sin_theta,
        sin(phi) * sin_theta,
        cos_theta);
}

/** Returns the index of the largest element */
int indexOfMaxComponent(vec3 v) { return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : (v.z > v.y) ? 2 : 1; }

/** Returns the index of the largest element */
int indexOfMaxComponent(vec2 v) { return (v.x > v.y) ? 0 : 1; }

/** Returns zero */
int indexOfMaxComponent(float v) { return 0; }

#endif
