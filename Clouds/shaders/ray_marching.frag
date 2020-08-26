#version 450 core 

#pragma compile(on)


struct Weather{
	float cloud_coverage;
	float cloud_type;
	float percipitation;
	vec3 wind_direciton;
	float cloud_speed;
};

layout(binding = 0 ) uniform sampler3D cloudNoiseLowFreq;

const float _4_PI = 12.566370614359172953850573533118; 
uniform vec3 camPos;
uniform vec3 stepSize;
uniform vec3 dt;
const int MAX_SAMPLES = 300;
const vec3 texMin = vec3(0);
const vec3 texMax = vec3(1);
uniform vec3 bMin;
uniform vec3 bMax;
uniform Weather weather;
uniform vec2 cloudMinMax;
uniform float eccentricity = 0.2;

in ncl_PerVertex{
	smooth vec3 pos;
	smooth vec3 uv;
};

layout(location = 0) out vec4 fragColor;

bool insideCube(vec3 pos){
	return dot(sign(pos - texMin), sign(texMax - pos)) == 3;
}

float remap(float x, float a, float b, float c, float d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

vec3 remap(vec3 x, vec3 a, vec3 b, vec3 c, vec3 d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

float henyeyGreenstein(vec3 lightDir, vec3 viewDir, float g){
	vec3 L = normalize(lightDir);
	vec3 V = normalize(viewDir);
	float _2gcos0 = 2 * g * dot(L, V);
	float gg = g * g;
	float num = 1 - gg;
	float denum = _4_PI * pow(1 + gg - _2gcos0, 1.5);

	return num / denum;
}

//vec2 curlNoise(vec2 uv);

vec3 sampleCoord(vec3 p){
	return remap(p, bMin, bMax, texMin, texMax);
}

float saturate(float val){
	return clamp(val, 0, 1);
}

float heightFractionForPoint(vec3 pos, vec2 cloudMinMax){
	float height_fraction = (pos.y - bMin.y);
	height_fraction /= (bMax.y - bMin.y);

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


float sampleCloudDensity(vec3 p, Weather weather){
	
//	vec3 densityCoord = remap(p, vec3(cloudMinMax.x), vec3(cloudMinMax.y), vec3(0.0), vec3(1.0));
	vec3 densityCoord = sampleCoord(p);

	vec4 low_frequency_noise = textureLod(cloudNoiseLowFreq, densityCoord, 0);
	float perlinWorley = low_frequency_noise.r;
	vec3 worley_low_freq = low_frequency_noise.gba;
	float low_freq_fbm = dot(worley_low_freq, vec3(0.625, 0.25, 0.125));

	float base_cloud = remap(perlinWorley, low_freq_fbm - 1.0, 1.0, 0.0, 1.0);

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;


	float cloud_coverage = weather.cloud_coverage;

	float base_cloud_with_coverage = remap(base_cloud, cloud_coverage, 1.0, 0.0, 1.0);

	base_cloud_with_coverage *= cloud_coverage;

	return base_cloud_with_coverage;
}

float sampleCloud(vec3 p){
	vec3 coord = sampleCoord(p);
	float perlinWorley = texture(cloudNoiseLowFreq, coord).x;
	vec3 worley = texture(cloudNoiseLowFreq, coord).yzw;

	float wfbm = dot(worley, vec3(0.625, 0.125, 0.25));

    // cloud shape modeled after the GPU Pro 7 chapter
    float cloud = remap(perlinWorley, wfbm - 1.0, 1.0, 0.0, 1.0);
    cloud = remap(cloud, 0.8, 1.0, 0., 1.0); // fake cloud coverage

	return cloud;
}

float lightEnergy(float sampleDensity, float percipitation, float eccentricity, vec3 samplePos, vec3 camPos, vec3 lightPos){
	float d = sampleDensity;
	float p = percipitation;
	float g = eccentricity;
	vec3 lightDir = lightPos - samplePos;
	vec3 viewDir = camPos - samplePos;
	float hg = henyeyGreenstein(lightDir, viewDir, g);

	return 2.0 * exp(-d * p) * (1 - exp(-2 * d)) * hg;
}

void main(){
	

	vec3 dataPos = pos;

	vec3 geomDir = normalize(pos - camPos);
	vec3 stepSize = (bMax-bMin)/textureSize(cloudNoiseLowFreq, 0);
	vec3 dirStep = geomDir * stepSize;

	bool stop = false;

	for(int i = 0; i < MAX_SAMPLES; i++){
		dataPos += dirStep;

		stop = dot(sign(dataPos - bMin), sign(bMax - dataPos)) < 3;

		if(stop) break;

		//float density = sampleCloud(dataPos);
		float density = sampleCloudDensity(dataPos, weather);

		//;

		float prev_alpha = density - (density * fragColor.a);
		fragColor.rgb = prev_alpha * vec3(density) + fragColor.rgb;
		fragColor.a += prev_alpha;

		if(fragColor.a > 0.99) break; 
	}

//	fragColor.rgb *= lightEnergy(fragColor.a);
}

//#pragma include("noise.glsl")