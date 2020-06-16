#pragma include("constants.glsl")
#pragma include("units.glsl")

const int CCW = -1;	// counterclockwise
const int CW = 1; // clockwise

vec3 Checkerboard(vec2 uv)
{
	return vec3(mod(floor(uv.x * 4.0) + floor(uv.y * 4.0), 2.0) < 1.0 ? 1.0 : 0.4);
}

float rng(vec2 st) {
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void swap(inout vec4 a, inout vec4 b) {
	vec4 temp = a;
	a = b;
	b = temp;
}

void swap(inout vec3 a, inout vec3 b) {
	vec3 temp = a;
	a = b;
	b = temp;
}


void swap(inout float a, inout float b) {
	float temp = a;
	a = b;
	b = temp;
}

vec3 flipNormal(vec3 wo, vec3 n) {
	return dot(wo, n) < 0.0 ? -n : n;
}

float saturate(float x) {
	return clamp(x, 0, 1);
}