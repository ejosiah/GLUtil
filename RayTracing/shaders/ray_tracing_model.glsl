#pragma include("ray_tracing_common.glsl")

const int SPHERE_SHAPE = 0;
const int CYLINDER = 1;
const int BOX = 2;
const int TRIANGLE = 3;
const int PLANE = 4;
const float INFINITY = 1.0 / 0.0;

struct Ray {
	vec4 origin;
	vec4 direction;
	float tMax;
};


struct Plane {
	vec3 n;
	float d;
	int id;
	int matId;
};

struct Box {
	vec3 min;
	vec3 max;
};

struct Sphere {
	vec3 c;
	vec3 color;
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
	int id;	// TODO fix offset issue
	int objectToWorldId;
	int worldToObjectId;
	int matId;
	vec3 a;
	vec3 b;
	vec3 c;
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
	int objId;
	int objType;
};

struct HitInfo {
	float t;
	int shape;
	int id;
	vec4 extras;
};

Ray transform(mat4 m, Ray ray) {
	Ray r;
	r.origin = (m * vec4(ray.origin.xyz, 1));
	r.direction = vec4(mat3(m) * ray.direction.xyz, 1);
	r.tMax = ray.tMax;
	return r;
}