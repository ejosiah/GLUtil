#pragma include("ray_tracing_common.glsl")
#pragma include("reflection.glsl")

const int SPHERE_SHAPE = 0;
const int CYLINDER = 1;
const int BOX = 2;
const int TRIANGLE = 3;
const int PLANE = 4;
const float INFINITY = 1.0 / 0.0;

struct Ray {
	vec3 origin;
	vec3 direction;
	float tMax;
};


struct Plane {
	vec4 n;
	vec4 min;
	vec4 max;
	float d;
	int id;
	int matId;
};

struct Box {
	vec3 min;
	vec3 max;
};

struct Sphere {
	vec4 c;
	vec4 color;
	mat4 objectToWorld;
	mat4 worldToObject;
	float r;
	float yMin;
	float yMax;
	float thetaMin;
	float thetaMax;
	float phiMax;
	int id;
	int matId;
};

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 kr;
	vec4 kt;
	float shine;
	float ior;
	int nBxDfs;
	int bsdf[8];
};

struct Cylinder {
	mat4 objectToWorld;
	mat4 worldToObject;
	float r;
	float yMin;
	float yMax;
	float phiMax;
	int id;
};

struct Triangle {
	vec4 a;
	vec4 b;
	vec4 c;
	int objectToWorldId;
	int worldToObjectId;
	int id;
	int matId;
};


struct Shading {
	vec3 n0;
	vec3 n1;
	vec3 n2;
	vec3 t0;
	vec3 t1;
	vec3 t2;
	vec3 bi0;
	vec3 bi1;
	vec3 bi2;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
	int id;
};

struct Camera {
	mat4 cameraToWorld;
	mat4 cameraToScreen;
	mat4 rasterToCamera;
	mat4 screenToRaster;
	mat4 rasterToScreen;
	float shutterOpen;
	float shutterClose;
	float lensRadius;
	float focalDistance;
};

struct CameraSample {
	vec2 pFilm;
	vec2 pLens;
	float time;
};

struct SurfaceInteration {
	vec3 n;
	vec3 p;
	vec2 uv;
	vec3 dpdu;
	vec3 dpdv;
	vec3 sn;
	vec3 st;
	vec3 sb;
	vec4 color;
	int matId;
	int shape;
	int shapeId;
	float n1;	// index of refraction above surface
	float n2; // index of refration below the surface
};

struct HitInfo {
	float t;
	int shape;
	int id;
	vec4 extras;
};

struct Debug {
	vec4 o;
	vec4 d;
	vec4 n;
	vec4 wo;
	vec4 wi;
	float f;
	float cos0;
	float n1;
	float n2;
	int bounce;
	int id;
	int shapeId;
	float t;
};

Ray transform(mat4 m, Ray ray) {
	vec4 o = m * ray.origin;
	vec3 d = mat3(m) * ray.direction.xyz;
	Ray r;
	r.origin = o;
	r.direction = vec4(d, 1);
	r.tMax = ray.tMax;
	return r;
}

Ray spawnRay(SurfaceInteration interact, vec3 wi) {
	Ray ray;
	ray.origin = vec4(interact.p + 0.0001 * wi, 1.0);
	ray.direction = vec4(wi, 1.0);
	ray.tMax = 1 * kilometer;
	return ray;
}

vec3 pointOnRay(Ray ray, HitInfo hit) {
	return ray.origin.xyz + hit.t * ray.direction.xyz;
}

struct TriangleData {
	samplerBuffer triangles;
	samplerBuffer normals;
//	samplerBuffer uvs;
	isamplerBuffer indices;
	bool hasIndices;
	int matId;
	int numTriangles;
};