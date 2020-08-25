#version 450 core

const int MAX_SAMPLES = 10;


struct Weather{
	float cloud_coverage;
	float cloud_type;
	float percipitation;
	vec3 wind_direciton;
	float cloud_speed;
};


layout(binding = 0) uniform sampler3D cloudNoiseLowFreq;
layout(binding = 1) uniform sampler3D clouldNoiseHighFreq;
layout(binding = 2) uniform sampler2D diffuseTexture;

uniform Weather weather;
uniform vec2 cloudMinMax;
uniform vec3 camPos;
uniform vec3 stepSize;
uniform vec3 texMin = vec3(-0.5);
uniform vec3 texMax = vec3(0.5);
uniform float cloud_coverage;

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

	//return saturate(height_fraction);
	return 0.5;
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

vec3 remap(vec3 x, vec3 a, vec3 b, vec3 c, vec3 d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

float sampleCloudDensity(vec3 p, Weather weather){
	
//	vec3 voxelCoord = remap(p, vec3(cloudMinMax.x), vec3(cloudMinMax.y), vec3(0.0), vec3(1.0));
	vec3 voxelCoord = mod(p, 128)/128;

	vec4 low_frequency_noise = textureLod(cloudNoiseLowFreq, voxelCoord, 0);
	float perlinWorley = low_frequency_noise.r;
	vec3 worley_low_freq = low_frequency_noise.gba;
	float low_freq_fbm = dot(worley_low_freq, vec3(0.625, 0.25, 0.125));

	float base_cloud = remap(perlinWorley, low_freq_fbm - 1.0, 1.0, 0.0, 1.0);

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;


//	float cloud_coverage = weather.cloud_coverage;
	float cloud_coverage = 0.85;

	float base_cloud_with_coverage = remap(base_cloud, cloud_coverage, 1.0, 0.0, 1.0);

	base_cloud_with_coverage *= cloud_coverage;

	return base_cloud_with_coverage;
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

bool insideCube(vec3 pos){
	return dot(sign(pos - texMin), sign(texMax - pos)) == 3;
}

vec4 traceRay(vec3 worldPos, vec3 camPos, Weather weather){
	vec3 viewDir = worldPos - camPos;
	vec3 viewDir_norm = normalize(viewDir);
	
//	vec3 pos = camPos;

//	if(insideCube(camPos)) return vec4(1, 0, 0, 1);

	//vec3 pos = insideCube(camPos) ? camPos : worldPos;
	vec3 pos = worldPos;
	vec3 dirStep = viewDir_norm * stepSize;

	bool stop = false;
	vec4 cloud_color = vec4(0);
	for(int i = 0; i < MAX_SAMPLES; i++){
		pos += dirStep;

		if(!insideCube(pos)) break;

		float cloud_density = sampleCloudDensity(pos, weather);
		cloud_density = cloud_density < 0 ? 0 : cloud_density;

		float prev_alpha = cloud_density - (cloud_density * cloud_color.a);
		cloud_color.rgb += prev_alpha * vec3(cloud_density);
		cloud_color.a += prev_alpha;

		if(cloud_color.a > 0.99) break; // fully saturated, 
	}
	return cloud_color;
}


out vec4 fragColor;

void main(){
	vec4 src = traceRay(vertex.position, camPos, weather);
	vec3 skyColor = vec3(0.1, 0.5, 0.9);
	//vec3 dest = texture(diffuseTexture, uv).rgb;
	//fragColor.rgb = src.rgb + (1 - src.a) * skyColor;
	fragColor = src;
}