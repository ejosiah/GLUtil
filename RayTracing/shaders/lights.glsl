const int DELTA_POSITION = 1 << 0;
const int DELTA_DIRECTION = 1 << 2;
const int AREA_LIGHT = 1 << 3;
const int INFINITE_LIGHT = 1 << 4;
const int POINT_LIGHT = (1 << 5) | DELTA_POSITION;
const int SUN_LIGHT = (1 << 6) | DELTA_DIRECTION;
const int DISTANT_LIGHT = (1 << 7) | DELTA_DIRECTION;
const float SUN_ANGULAR_RADIUS = 0.262743736528799;

struct LightSource {
	vec4 I;
	vec3 position;
	mat4 lightToWorld;
	mat4 worldToLight;
	int flags;
	int nSamples;
};


bool isDeltaLight(int flags) {
	return (flags & DELTA_POSITION) > 0 || (flags & DELTA_DIRECTION) > 0;
}

vec3 sample_point_light(LightSource light, SurfaceInteration ref, vec2 u, out vec3 wi, out float pdf, out Ray shadowRay) {
	vec3 lightDir = light.position.xyz - ref.p;
	wi = normalize(lightDir);
	pdf = 1.0;
	shadowRay.origin = vec4(ref.p + wi * 0.001, 1);
	shadowRay.direction = vec4(wi, 1);
	shadowRay.tMax = 1000;

	//return light.I.rgb * light.I.a / (4 * dot(lightDir, lightDir) * PI);
	return (light.I.rgb * light.I.a)  / dot(lightDir, lightDir);
}

vec3 sample_distant_light(LightSource light, SurfaceInteration ref, vec2 u, out vec3 wi, out float pdf, out Ray shadowRay) {
	wi = light.position.xyz;
	pdf = 1;
	shadowRay.origin = vec4(ref.p, 1);
	shadowRay.direction = vec4( normalize(ref.p - (ref.p + wi * 2 * worldRadius)), 1);
	shadowRay.tMax = 1000;
	return light.I.rgb * light.I.a;
}


vec3 sample_Li(LightSource light, SurfaceInteration ref, vec2 u, out vec3 wi, out float pdf, out Ray shadowRay) {
	if ((light.flags & POINT_LIGHT) > 0) {
		return sample_point_light(light, ref, u, wi, pdf, shadowRay);
	}
	else if ((light.flags & DISTANT_LIGHT) > 0) {
		return sample_distant_light(light, ref, u, wi, pdf, shadowRay);
	}
	else {
		return vec3(0);
	}
}

bool isBlack(vec3 L) {
	bvec3 res = equal(L, vec3(0));
	return all(res);
}

vec3 power(LightSource light) {
	if ((light.flags & POINT_LIGHT) > 0) {
		return 4 * PI * light.I.xyz;
	}
	else {
		return vec3(0);
	}
}