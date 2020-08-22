#pragma include("perlin.glsl")

float fbm_perlin(float x, float y, float z, int octaves, int period)
{
	float value = 0.0;
	int n = octaves - 1;
	float max_value = 0.0;
	for (int i = 0; i <= n; i++) {
		float freq = pow(2.0, i);
		float amp = pow(0.5, i);
		value += amp * perlin_noise(x * freq, y * freq, z * freq, period * freq);
		max_value += amp;
	}

	return value / max_value;
}