float ditherPattern[4][4] = { { 0.0f, 0.5f, 0.125f, 0.625f},
{ 0.75f, 0.22f, 0.875f, 0.375f},
{ 0.1875f, 0.6875f, 0.0625f, 0.5625},
{ 0.9375f, 0.4375f, 0.8125f, 0.3125} };

bool insideCube(vec3 pos){
	return dot(sign(pos - texMin), sign(texMax - pos)) == 3;
}

float remap(float x, float a, float b, float c, float d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

vec3 remap(vec3 x, vec3 a, vec3 b, vec3 c, vec3 d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

vec2 remap(vec2 x, vec2 a, vec2 b, vec2 c, vec2 d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

float henyeyGreenstein(vec3 lightDir, vec3 viewDir, float g){
	vec3 L = normalize(lightDir);
	vec3 V = normalize(viewDir);
	float _2gcos0 = 2 * g * max(0, dot(L, V));
	float gg = g * g;
	float num = 1 - gg;
	float denum = _4_PI * pow(1 + gg - _2gcos0, 1.5);

	return num / denum;
}

vec2 curlNoise(vec2 uv);

float mipLevel = 0;

vec3 sampleCoord(vec3 p){
//	return remap(p, vec3(-EARTH_RADIUS, CLOUDS_START, EARTH_RADIUS), vec3(-EARTH_RADIUS, CLOUDS_END, EARTH_RADIUS), vec3(0), vec3(4));
	return p * 0.0005;
}

vec2 weatherSampleCoord(vec3 p) {
	return remap(p.xz, bMin.xz, bMax.xz, vec2(0), vec2(1));
}


float saturate(float val){
	return clamp(val, 0, 1);
}

float heightFractionForPoint(vec3 pos, vec2 cloudMinMax){
	float height_fraction = (pos.y - cloudMinMax.x);
	height_fraction /= (cloudMinMax.y - cloudMinMax.x);

	return saturate(height_fraction);
}

float densityHeightGradientForPoint(vec3 p, Weather weather){
	
	float height = heightFractionForPoint(p, cloudMinMax);
	//float cloud_type = weather.cloud_type;
	vec2 uv = weatherSampleCoord(p);
	float cloud_type = texture(weatherData, uv).b;

	const vec4 stratusGrad = vec4(0.02f, 0.05f, 0.09f, 0.11f);
	const vec4 stratocumulusGrad = vec4(0.02f, 0.2f, 0.48f, 0.625f);
	const vec4 cumulusGrad = vec4(0.01f, 0.0625f, 0.78f, 1.0f);
	float stratus = 1.0f - clamp(cloud_type * 2.0f, 0, 1);
	float stratocumulus = 1.0f - abs(cloud_type - 0.5f) * 2.0f;
	float cumulus = clamp(cloud_type - 0.5f, 0, 1) * 2.0f;
	vec4 cloudGradient = stratusGrad * stratus + stratocumulusGrad * stratocumulus + cumulusGrad * cumulus;
	return smoothstep(cloudGradient.x, cloudGradient.y, height) - smoothstep(cloudGradient.z, cloudGradient.w, height);
}


float sampleCloudDensity(vec3 p, Weather weather, float mipLevel){
	
	vec3 densityCoord = sampleCoord(p);

	vec4 low_frequency_noise = textureLod(cloudNoiseLowFreq, densityCoord, mipLevel);
	float perlinWorley = low_frequency_noise.r;
	vec3 worley_low_freq = low_frequency_noise.gba;
	float low_freq_fbm = dot(worley_low_freq, vec3(0.625, 0.25, 0.125));

	float base_cloud = remap(perlinWorley, low_freq_fbm - 1.0, 1.0, 0.0, 1.0);

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;

	vec2 uv = weatherSampleCoord(p);
//	float cloud_coverage = weather.cloud_coverage;
	float cloud_coverage = texture(weatherData, uv).r;

	float base_cloud_with_coverage = remap(base_cloud, 1 - cloud_coverage, 1.0, 0.0, 1.0);

	base_cloud_with_coverage *= cloud_coverage;

   float height_fraction = heightFractionForPoint(p, cloudMinMax);


	vec2 curl = curlNoise(densityCoord.xy * dt);
	p.xz = curl * (1 - height_fraction);

	densityCoord = sampleCoord(p);

	vec3 high_frequency_noise = textureLod(cloudNoiseHighFreq, densityCoord * 0.1, mipLevel).rgb;

	float high_freq_fbm = dot(high_frequency_noise, vec3(0.625, 0.25, 0.125));

	height_fraction = heightFractionForPoint(p, cloudMinMax);

	float high_freq_noise_modifier = mix(high_freq_fbm, 1 - high_freq_fbm, saturate(height_fraction * 10));

	float final_cloud = remap(base_cloud_with_coverage, high_freq_noise_modifier * 0.2, 1.0, 0.0, 1.0);

	return final_cloud;
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

const vec3 noise_kernel[] = {
	vec3(-0.316253, 0.147451, -0.902035),
	vec3(0.208214, 0.114857, -0.669561),
	vec3(-0.398435, -0.105541, -0.722259),
	vec3(0.0849315, -0.644174, 0.0471824),
	vec3(0.470606, 0.99835, 0.498875),
	vec3(-0.207847, -0.176372, -0.847792)
};

float sampleCloudDensityAlongCone(vec3 samplePos, vec3 direction){
	vec3 lightStep = direction * 0.1;
	float coneSpreadMultiplier = length(lightStep);
	int lod = -1;

	float density = 0;
	vec3 p = samplePos;
	for(int i = 0; i < 6; i++){
		p += lightStep * (coneSpreadMultiplier * noise_kernel[i] * float(i));

		density += sampleCloudDensity(p, weather, mipLevel + 1);
	}

	return density;
}

float hash13(vec3 p3)
{
	p3 = fract(p3 * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}
vec4 matchCloud(vec3 origin, vec3 direction, vec3 exitPoint, ivec2 pixelPos){
	
	float ditherValue = ditherPattern[pixelPos.x % 4][pixelPos.y % 4];
	vec3 windDir = -vec3(1, 0, 0);
	float cloud_speed = 100;
	float cloud_top_offset = 10.0;
	float height_fraction = heightFractionForPoint(origin, cloudMinMax);
	vec3 p = origin; // +(direction / ditherValue);
	p +=  height_fraction * windDir * cloud_top_offset;
	p += (windDir + vec3(0, 0, 0)) * dt * cloud_speed;
	
	vec4 fragColor = vec4(0);

	vec3 geomDir = normalize(direction);
	//float samples = textureSize(cloudNoiseLowFreq, int(mipLevel)).x;
	float samples = 256 ;
	//vec3 stepSize = abs(bMax- bMin)/vec3(textureSize(cloudNoiseLowFreq, int(mipLevel)));
	vec3 stepSize = abs(exitPoint - origin) / samples;
	vec3 dirStep = geomDir * stepSize;
	vec3 dataPos = p;

	bool stop = false;

	for(int i = 0; i < samples; i++){
		dataPos += dirStep;

	//	stop = dot(dataPos, dataPos) > dot(exitPoint, exitPoint);

		if(stop) break;

		//float density = sampleCloud(dataPos);
		float density = sampleCloudDensity(dataPos, weather, mipLevel);
		density = clamp(density, 0, 1);

		//;
		float energy = 1;
		if(density != 0){
			vec2 uv = weatherSampleCoord(p);
			//float precipitation = texture(weatherData, uv).g;
			float precipitation = weather.precipitation;
			float d = sampleCloudDensityAlongCone(dataPos, dirStep);
			energy = 200 * lightEnergy(d, precipitation, eccentricity, dataPos, camPos, lightPos);
		}

		float prev_alpha = density - (density * fragColor.a);
		fragColor.rgb += energy * prev_alpha * vec3(density);
		fragColor.a += prev_alpha;

		if(fragColor.a > 0.99) break; 
	}

	fragColor.rgb /= 1 + fragColor.rgb;

	return fragColor;
}

#pragma include("noise.glsl")