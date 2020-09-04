bool insideCube(vec3 pos){
	return dot(sign(pos - texMin), sign(texMax - pos)) == 3;
}

float remap(float x, float a, float b, float c, float d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}


vec2 remap(vec2 x, vec2 a, vec2 b, vec2 c, vec2 d) {
	return (((x - a) / (b - a)) * (d - c)) + c;
}

vec3 remap(vec3 x, vec3 a, vec3 b, vec3 c, vec3 d){
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

vec3 sampleCoord(vec3 p){
	//return remap(p, bMin, bMax, vec3(0), vec3(5));
	return p/1000;
}

vec2 weatherSampleCoord(vec3 p) {
	return remap(p.xz, -vec2(tMax), vec2(tMax), vec2(0), vec2(1));
}

float saturate(float val){
	return clamp(val, 0, 1);
}

//float heightFractionForPoint(vec3 pos, vec2 cloudMinMax){
//	float height_fraction = (pos.y - bMin.y);
//	height_fraction /= (bMax.y - bMin.y);
//
//	return saturate(height_fraction);
//}

float heightFractionForPoint(vec3 pos) {
	float height_fraction = distance(pos, cloudMin) / distance(cloudMin, cloudMax);
	return saturate(height_fraction);
}

float densityHeightGradientForPoint(vec3 p, Weather weather){
	
	float height = heightFractionForPoint(p);
	float cloud_type = weather.cloud_type;
//	vec2 uv = weatherSampleCoord(p);
//	float cloud_type = texture(weatherData, uv).b;

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

	vec4 low_frequency_noise = textureLod(cloudNoiseLowFreq, densityCoord, 3);
	float perlinWorley = low_frequency_noise.r;
	vec3 worley_low_freq = low_frequency_noise.gba;
	float low_freq_fbm = dot(worley_low_freq, vec3(0.625, 0.25, 0.125));

	float base_cloud = remap(perlinWorley, low_freq_fbm - 1.0, 1.0, 0.0, 1.0);

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;


	float cloud_coverage = weather.cloud_coverage;
	//vec2 uv = weatherSampleCoord(p);
	//float cloud_coverage = texture(weatherData, uv).r;

	float base_cloud_with_coverage = remap(base_cloud, 1 - cloud_coverage, 1.0, 0.0, 1.0);

	base_cloud_with_coverage *= cloud_coverage;

//	return base_cloud_with_coverage;
	
   float height_fraction = heightFractionForPoint(p);

	vec2 curl = curlNoise(densityCoord.xy * dt);
	p.xz = curl * (1 - height_fraction);

	densityCoord = sampleCoord(p);

	vec3 high_frequency_noise = texture(cloudNoiseHighFreq, densityCoord * 0.1).rgb;

	float high_freq_fbm = dot(high_frequency_noise, vec3(0.625, 0.25, 0.125));

	height_fraction = heightFractionForPoint(p);

	float high_freq_noise_modifier = mix(high_freq_fbm, 1 - high_freq_fbm, saturate(height_fraction * 10));

	float final_cloud = remap(base_cloud_with_coverage, high_freq_noise_modifier * 0.2, 1.0, 0.0, 1.0);

	return final_cloud;
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

		density += sampleCloudDensity(p, weather);
	}

	return density;
}


vec4 matchCloud(vec3 origin, vec3 direction){
	
	vec3 windDir = vec3(1, 0, 0);
	float cloud_speed = 10.0;
	float cloud_top_offset = 10.0;
	float height_fraction = heightFractionForPoint(origin);
	vec3 p = origin + height_fraction * windDir * cloud_top_offset;
	p += (windDir + vec3(0, 0.1, 0)) * dt * cloud_speed;
	
	vec4 fragColor = vec4(0);
	//vec3 dataPos = p;
	vec3 dataPos = origin;
//	dataPos += (weather.wind_direciton + vec3(0, 0.1, 0)) * dt * weather.cloud_speed;

	vec3 geomDir = normalize(direction);
	//vec3 stepSize = (bMax-bMin)/textureSize(cloudNoiseLowFreq, 0);
	float samples = textureSize(cloudNoiseLowFreq, 0).x;
	vec3 stepSize = (cloudMax - cloudMin)/ samples;
	vec3 dirStep = geomDir * stepSize;

	bool stop = false;

	for(int i = 0; i < samples; i++){
		dataPos += dirStep;

	//	stop = dot(sign(dataPos - bMin), sign(bMax - dataPos)) < 3;

		if(stop) break;

		//float density = sampleCloud(dataPos);
		float density = sampleCloudDensity(dataPos, weather);
		density = clamp(density, 0, 1);

		//;
		float energy = 1;
		if(density != 0){
			float d = sampleCloudDensityAlongCone(dataPos, dirStep);
			//vec2 uv = weatherSampleCoord(dataPos);
			//float p = texture(weatherData, uv).g;
			float p = weather.precipitation;
			energy = 200 * lightEnergy(d, p, eccentricity, dataPos, camPos, lightPos);
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