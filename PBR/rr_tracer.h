#pragma once

#include <glm/glm.hpp>
#include "pbr_models.h"
#include "interaction.h"
using namespace glm;


int numSpheres = 1;

extern Sphere_pbr* spheres;
extern Plane_pbr* planes;
extern Material_pbr* materials;

bool intersectPlane(Ray ray, Plane_pbr p, HitInfo hit) {

	float t = p.d - dot(p.n, ray.o);
	t /= dot(p.n, ray.d);

	if (t < 0 || t > ray.tMax) {
		return false;
	}
	hit.t = t;
	hit.id = p.id;
	hit.shape = PLANE;
	return true;
}

inline bool intersectSphere(Ray ray, Sphere_pbr s, HitInfo hit) {
	float t0 = -1;
	float t1 = t0;

	Ray r = transform(s.worldToObject, ray);

	vec3 m = r.o - s.c;
	float a = dot(r.d, r.d);
	float b = dot(m, ray.d);
	float c = dot(m, m) - s.r * s.r;

	if (c > 0.0 && b > 0.0) return false;

	float discr = b * b - a * c;

	if (discr < 0.0) return false;
	float sqrtDiscr = sqrt(discr);

	t0 = (-b - sqrtDiscr) / a;
	t1 = (-b + sqrtDiscr) / a;
	if (t0 > t1) std::swap(t0, t1);

	float tHit = t0;
	if (tHit <= 0.0) tHit = t1;
	if (tHit > r.tMax) return false;

	vec3 p = r.o + r.d * tHit;
	p *= s.r / distance(p, s.c);
	if (p.x == 0 && p.z == 0) p.x = 1E-5 * s.r;
	float phi = atan(p.z, p.x);
	if (phi < 0.0) phi += TWO_PI;

	if ((s.yMin > -s.r && p.y < s.yMin) || (s.yMax < s.r && p.y > s.yMax) || phi > s.phiMax) {
		if (tHit == t1) return false;
		if (t1 > ray.tMax) return false;

		tHit = t1;
		p = r.o + r.d * tHit;
		p *= s.r / distance(p, s.c);
		if (p.x == 0.0 && p.z == 0.0) p.x = 1E-5 * s.r;
		float phi = atan(p.z, p.x);
		if (phi < 0.0) phi += TWO_PI;

		if ((s.yMin > -s.r && p.y < s.yMin)
			|| (s.yMax < s.r && p.y > s.yMax)
			|| phi > s.phiMax) return false;
	}

	hit.t = tHit;
	hit.shape = SPHERE_SHAPE;
	hit.id = s.id;
	hit.extras.x = phi;
	return true;
}

bool intersectScene(Ray ray, HitInfo hit) {
	hit.t = ray.tMax;
	HitInfo local_hit;
	local_hit.t = hit.t;
	bool aHit = false;
	for (int i = 0; i < numSpheres; i++) {
		Sphere_pbr s = spheres[i];
		if (intersectSphere(ray, s, local_hit)) {
			aHit = true;
			if (local_hit.t < hit.t) {
				hit = local_hit;
			}
		}
	}

	//local_hit.t = hit.t;
	//if (intersectsTriangle(ray, local_hit, 0)) {
	//	aHit = true;
	//	if (local_hit.t < hit.t) {
	//		hit = local_hit;
	//	}
	//}

	local_hit.t = hit.t;
	if (intersectPlane(ray, planes[0], local_hit)) {
		aHit = true;
		if (local_hit.t < hit.t) {
			hit = local_hit;
		}
	}

	return aHit;
}

vec4 doIntersect(Ray ray, int maxBounce, vec3& wi, vec3& wo) {

	//	if (intersectScene(ray, hit)) {
	//		intialize(hit, ray, interact);
	//		return shade(interact, 0);
	//	}
	//	else {
	//		interact.matId = -1;
	//		return texture(skybox, ray.d);
	//	}

	vec4 color = vec4(0);
	vec3 f = vec3(1);
	vec3 L = vec3(0);
	Ray r = ray;
	float accum_pdf = 1.0;
	vec3 accum_f = vec3(1.0);
	for (int i = 0; i < maxBounce; i++) {
		HitInfo hit;
		vec3 wi;
		vec3 wo = -r.d;
		if (intersectScene(r, hit)) {
			SurfaceInteration interact;
			intialize(hit, r, interact);

			if (interact.shape == PLANE) {
				L += accum_f * shade(interact, 0).xyz / accum_pdf;
				color = vec4(0);
				break;
			}

			float pdf;
			vec2 u = hash23(interact.p);
			Ray shadowRay;

			for (int i = 0; i < nLights; i++) {
				vec3 Li = sample_Li(light[i], interact, u, wi, pdf, shadowRay);

				if (isBlack(Li) || pdf == 0) continue;

				vec3 f = bsdf_f(wo, wi, BSDF_ALL, interact);
				if (!isBlack(f)) {
					L += accum_f * f * Li * abs(dot(wi, interact.n)) / (pdf * accum_pdf);
				}
				u = hash22(u);
			}

			u = hash23(wo);

			accum_f *= Sample_f(wo, wi, u, pdf, SPECIULAR_REFLECT, interact);
			//accum_f *= Sample_f(wo, wi, u, pdf, SPECULAR_TRANSMISSION, interact);

			//if (i == 1) {
				//color = vec4(interact.p, float(gl_GlobalInvocationID.x * gl_GlobalInvocationID.y));
				//break;
			//}


			accum_pdf *= pdf;
			r.o = interact.p + 0.001 * wi;
			r.d = wi;

			//if (i == (MAX_BOUNCES - 1)) L = vec3(1, 0, 0);


		}
		else {
			L += vec3(1);
			color = vec4(0);
			break;
		}
	}
	return vec4(L, 1.0);
	//	return color;
}