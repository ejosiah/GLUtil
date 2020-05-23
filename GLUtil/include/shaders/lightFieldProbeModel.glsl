//layout(binding = 4) uniform sampler2DArray radianceProbeGrid;
//layout(binding = 5) uniform sampler2DArray normalProbeGrid;
//layout(binding = 6) uniform sampler2DArray distanceProbeGrid;
//layout(binding = 7) uniform sampler2DArray lowResolutionDistanceProbeGrid;
//layout(binding = 8) uniform samplerCubeArray irradianceProbeGrid;
//layout(binding = 9) uniform samplerCubeArray meanDistProbeGrid;

#define Vector3int32 ivec3

#define Point2 vec2
#define Vector2 vec2

#define Point3 vec3
#define Vector3 vec3

struct Texture2DArray{
    sampler2DArray sampler;
    vec3 size;
    vec3 invSize;
    vec4 readMultiplyFirst;
    vec4 readAddSecond;
    bool notNull;
};

struct TextureCubeArray{
    samplerCubeArray sampler;
    vec2 size;
    vec2 invSize;
    vec4 readMultiplyFirst;
    vec4 readAddSecond;
    bool notNull;
};

struct LightFieldSurface {
    Texture2DArray          radianceProbeGrid;
    Texture2DArray          normalProbeGrid;
    Texture2DArray          distanceProbeGrid;
    Texture2DArray          lowResolutionDistanceProbeGrid;
    Vector3int32            probeCounts;
    Point3                  probeStartPosition;
    Vector3                 probeStep;
    int                     lowResolutionDownsampleFactor;
    TextureCubeArray        irradianceProbeGrid;
    TextureCubeArray        meanDistProbeGrid;
};



//uniform Texture2DArray radianceProbeGrid1;
//uniform Texture2DArray normalProbeGrid0;
uniform LightFieldSurface lightFieldSurface;
