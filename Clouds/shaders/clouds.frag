#version 450 core

const int MAX_SAMPLES = 1;


struct Weather{
	float cloud_coverage;
	float cloud_type;
	float percipitation;
	vec3 wind_direciton;
	float cloud_speed;
};


layout(binding = 0) uniform sampler3D cloudNoiseLowFreq;
layout(binding = 1) uniform sampler3D clouldNoiseHighFreq;

uniform Weather weather;
uniform vec2 cloudMinMax;
uniform vec3 camPos;
uniform vec3 stepSize;
uniform vec3 texMin = vec3(-0.5);
uniform vec3 texMax = vec3(0.5);

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;



float saturate(float val){
	return clamp(val, 0.0, 1.0);
}


float heightFractionForPoint(vec3 pos, vec2 cloudMinMax){
	float height_fraction = (pos.y - cloudMinMax.x);
	height_fraction /= (cloudMinMax.y - cloudMinMax.x);

	return saturate(height_fraction);
}

float densityHeightGradientForPoint(vec3 p, Weather weather){
	
	float height = heightFractionForPoint(p, cloudMinMax);
	float cloud_type = weather.cloud_type;

	const vec4 stratusGrad = vec4(0.02f, 0.05f, 0.09f, 0.11f);
	const vec4 stratocumulusGrad = vec4(0.02f, 0.2f, 0.48f, 0.625f);
	const vec4 cumulusGrad = vec4(0.01f, 0.0625f, 0.78f, 1.0f);
	float stratus = 1.0f - clamp(cloud_type * 2.0f, 0, 1);
	float stratocumulus = 1.0f - abs(cloud_type - 0.5f) * 2.0f;
	float cumulus = clamp(cloud_type - 0.5f, 0, 1) * 2.0f;
	vec4 cloudGradient = stratusGrad * stratus + stratocumulusGrad * stratocumulus + cumulusGrad * cumulus;
	return smoothstep(cloudGradient.x, cloudGradient.y, height) - smoothstep(cloudGradient.z, cloudGradient.w, height);
}

float remap(float x, float a, float b, float c, float d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

float sampleCloudDensity(vec3 p, Weather weather){
	// low_frequency_noise
	// hight_frequency_noise
	vec4 low_frequency_noise = textureLod(cloudNoiseLowFreq, p, 0);
	float perlinWorley = low_frequency_noise.r;
	vec3 worley_low_freq = low_frequency_noise.gba;
	float low_freq_fbm = dot(worley_low_freq, vec3(0.625, 0.25, 0.125));

	float base_cloud = remap(perlinWorley, low_freq_fbm - 1.0, 1.0, 0.0, 1.0);
//	base_cloud = remap(base_cloud, 0.3, 1.0, 0., 1.0); // fake cloud coverage

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;

	return base_cloud;
	//return perlinWorley;
}

//float sampleCloudDensity(vec3 p, Weather weather){
//
//
//	float perlinWorley = texture(cloudNoiseLowFreq, p).x;
//	vec3 worley = texture(cloudNoiseLowFreq, p).yzw;
//
//	float wfbm = dot(worley, vec3(0.625, 0.125, 0.25));
//
//    // cloud shape modeled after the GPU Pro 7 chapter
//    float cloud = remap(perlinWorley, wfbm - 1.0, 1.0, 0.0, 1.0);
//    cloud = remap(cloud, 0.85, 1.0, 0., 1.0); // fake cloud coverage
//
//	return cloud;
//}

vec4 traceRay(vec3 worldPos, vec3 camPos, Weather weather){
	vec3 viewDir = worldPos - camPos;
	vec3 viewDir_norm = normalize(viewDir);
	
//	vec3 pos = camPos;
	vec3 pos = worldPos;
	vec3 dirStep = viewDir_norm * stepSize;

	bool stop = false;
	vec4 cloud_color = vec4(0);
	for(int i = 0; i < MAX_SAMPLES; i++){
		pos += dirStep;

//		stop = dot(pos, pos) > pow(cloudMinMax.y, 2);
		stop = dot(sign(pos - texMin), sign(texMax - pos)) < 3;
		if(stop) break;

		float cloud_density = sampleCloudDensity(pos, weather);
		float prev_alpha = cloud_density - (cloud_density * cloud_color.a);
		cloud_color.rgb += prev_alpha * vec3(cloud_density);
		cloud_color.a == prev_alpha;

		if(cloud_color.a > 0.99) break; // fully saturated, 
	}

	return cloud_color;
}


out vec4 fragColor;

void main(){
	vec3 color = traceRay(vertex.position, camPos, weather).rgb;
	vec3 skyCol = vec3(0.1, 0.5, 0.9);
	fragColor = vec4(color, 1);
}