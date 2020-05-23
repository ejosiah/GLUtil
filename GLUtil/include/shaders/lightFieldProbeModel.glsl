#pragma include("math.glsl")

const float minThickness = 0.03; // meters
const float maxThickness = 0.50; // meters

// Points exactly on the boundary in octahedral space (x = 0 and y = 0 planes) map to two different
// locations in octahedral space. We shorten the segments slightly to give unambigous locations that lead
// to intervals that lie within an octant.
const float rayBumpEpsilon = 0.001; // meters

// If we go all the way around a cell and don't move farther than this (in m)
// then we quit the trace
const float minProgressDistance = 0.01;

//  zyx bit pattern indicating which probe we're currently using within the cell on [0, 7]
#define CycleIndex int

// On [0, L.probeCounts.x * L.probeCounts.y * L.probeCounts.z - 1]
#define ProbeIndex int

// probe xyz indices
#define GridCoord ivec3

// Enumerated value
#define TraceResult int
#define TRACE_RESULT_MISS    0
#define TRACE_RESULT_HIT     1
#define TRACE_RESULT_UNKNOWN 2


float distanceSquared(Point2 v0, Point2 v1) {
    Point2 d = v1 - v0;
    return dot(d, d);
}

struct Ray {
    vec3 direction;
    vec3 origin;
};


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