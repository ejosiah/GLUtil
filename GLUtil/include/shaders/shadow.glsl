layout(binding = 4) uniform sampler2D shadowMap;
layout(binding = 5) uniform samplerCube pointShadowMap;

const int DIRECTIONAL_SHADOW = 0;
const int POINT_SHADOW = 1;


uniform bool shadowOn = false;
uniform float farPlane;
uniform int shadowType;


float ShadowCalculationDirectional(vec4 pos, float NdotL) {
    if (!shadowOn) return 0.0;

    vec3 projCoords = pos.xyz / pos.w;
    projCoords = projCoords * 0.5 + 0.5; // [-1, 1] to [0, 1]

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - NdotL), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculationPoint(vec3 pos, vec3 lightPos, vec3 viewPos, float NdotL) {
    if (!shadowOn) return 0.0;
    vec3 fragToLight = pos - lightPos;

    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - pos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    

    return shadow;
}

float ShadowCalculation(vec3 posInWorld, vec4 posInLight, vec3 lightPos, vec3 viewPos, float NdotL) {
    if (!shadowOn) return 0.0;
    if (shadowType == DIRECTIONAL_SHADOW) return ShadowCalculationDirectional(posInLight, NdotL);
    if (shadowType == POINT_SHADOW) return ShadowCalculationPoint(posInWorld, lightPos, viewPos, NdotL);
    return 0.0;
}
