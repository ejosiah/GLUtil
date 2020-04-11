#pragma once

#include <glm/glm.hpp>

#define PI                    3.1415926535897932384626422832795028841971
#define INV_PI				  0.31830988618379067153776752674503
#define TWO_PI				  6.2831853071795864769252867665590057683943
#define SQRT_OF_ONE_THIRD     0.5773502691896257645091487805019574556476
#define EPSILON				  0.000001

using namespace glm;

const int BSDF_REFLECTION = 1 << 0;
const int BSDF_TRANSMISSION = 1 << 1;
const int BSDF_DIFFUSE = 1 << 2;
const int BSDF_GLOSSY = 1 << 3;
const int BSDF_SPECULAR = 1 << 4;
const int BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION;

const int FRESNEL_NOOP = 1 << 0;
const int FRESNEL_DIELECTRIC = 1 << 1;
const int FRESNEL_CONDOCTOR = 1 << 2;

const int SPHERE_SHAPE = 0;
const int CYLINDER = 1;
const int BOX = 2;
const int TRIANGLE = 3;
const int PLANE = 4;

struct Ray {
	vec3 o;
	vec3 d;
	float tMax;
	float time;
	//	Medium medium;

		// Ray differentials
	bool hasDefferntials;
	vec3 rxo, ryo;
	vec3 rxd, ryd;

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

struct LightSource {
	vec4 I;
	vec4 position;
	mat4 lightToWorld;
	mat4 worldToLight;
	int flags;
	int nSamples;
	int shapeId;
};


struct Plane_pbr {
	vec3 n;
	float d;
	int id;
	int matId;
};

struct Box_pbr {
	vec3 min;
	vec3 max;
};

struct Sphere_pbr {
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

struct Material_pbr {
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

inline Ray transform(mat4 m, Ray ray) {
	Ray r;
	r.o = vec3(m * vec4(ray.o, 1));
	r.d = mat3(m) * ray.d;
	r.tMax = ray.tMax;
	return r;
}
