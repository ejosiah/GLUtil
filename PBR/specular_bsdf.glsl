#pragma once

#include <glm/glm.hpp>
#include "pbr_models.h"

using namespace glm;

inline vec3 SpecularReflection_f(vec3 wo, vec3 wi) {
	return vec3(0.0);
}

inline vec3 SpecularReflection_Sample_f(vec3 wo, out vec3 wi, vec2 u, out float pdf, SurfaceInteration intaract) {
	/*
		not yet using reflection coordinate system
		wi.x = -wo.x;
		wi.z = -wo.z;
		wi.y = wo.y;
	*/

	wi = reflect(-wo, intaract.n);
	pdf = 1.0;

	Material m = material[intaract.matId];

	return m.kr.xyz * fresnel(vec3(0), vec3(0), vec3(0), 0.0, FRESNEL_NOOP);
	//	return vec3(0);
}

inline float SpecularReflection_Pdf(vec3 wi, vec3 wo) {
	return 0.0;
}


/******************** Transmission *********************/

inline vec3 SpecularTransmission_f(vec3 wo, vec3 wi) {
	return vec3(0.0);
}

inline vec3 flipNormal(vec3 wo, vec3 n) {
	return dot(wo, n) < 0.0 ? -n : n;
}

inline vec3 SpecularTransmission_Sample_f(vec3 wo, out vec3 wi, vec2 u, out float pdf, SurfaceInteration interact) {
	bool entering = dot(wo, interact.n) > 0;
	float etaI = entering ? interact.n1 : interact.n2;
	float etaT = entering ? interact.n2 : interact.n1;

	float eta = etaI / etaT;
	vec3 n = interact.n;	// TODO use reflection space normal
	n = flipNormal(wi, n);

	wi = refract(-wo, n, eta);

	bool totalInternalReflection = isnan(wi) == bvec3(true);
	if (totalInternalReflection) {
		pdf = 0;
		return vec3(0);
	}

	pdf = 1.0;
	float cos0i = dot(wi, n);

	Material m = material[interact.matId];
	vec3 rtVal = m.kt.xyz * (vec3(1) - fresnel(vec3(etaI), vec3(etaT), m.kr.xyz, cos0i, FRESNEL_DIELECTRIC));
	// TODO handle mode
	//return rtVal / abs(cos0i);
	return vec3(1.0);
}

inline float SpecularTransmission_Pdf(vec3 wi, vec3 wo) {
	return 0.0;
}