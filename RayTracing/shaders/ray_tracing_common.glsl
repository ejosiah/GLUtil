#pragma include("constants.glsl")

float rng(vec2 st) {
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}


void swap(inout float a, inout float b) {
	float temp = a;
	a = b;
	b = temp;
}