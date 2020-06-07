const int DELTA_POSITION = 1 << 0;
const int DELTA_DIRECTION = 1 << 2;
const int AREA_LIGHT = 1 << 3;
const int INFINITE_LIGHT = 1 << 4;
const int POINT_LIGHT = (1 << 5) | DELTA_POSITION;

struct LightSource {
	vec4 I;
	vec3 position;
	mat4 lightToWorld;
	mat4 worldToLight;
	int flags;
	int nSamples;
};

const float meters = 0.01;

bool isDeltaLight(int flags) {
	return (flags & DELTA_POSITION) > 0 || (flags & DELTA_DIRECTION) > 0;
}

vec3 sample_Li(LightSource light, SurfaceInteration ref, vec2 u, out vec3 wi, out float pdf, out Ray shadowRay) {
	if ((light.flags & POINT_LIGHT) > 0) {
		wi = normalize(light.position.xyz - ref.p);
		pdf = 1.0;
		shadowRay.o = light.position.xyz + 0.001 * ref.n;
		shadowRay.d = wi;
		shadowRay.tMax = 1000;

		float d = distance(light.position.xyz, ref.p);
		return light.I.xyz / (d * d);
	}
	else {
		return vec3(0);
	}
}

vec3 power(LightSource light) {
	if ((light.flags & POINT_LIGHT) > 0) {
		return 4 * PI * light.I.xyz;
	}
	else {
		return vec3(0);
	}
}