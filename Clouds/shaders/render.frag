#version 450 core 

layout(binding = 0) uniform sampler3D cloudNoiseLowFreq;

struct Weather{
	float cloud_coverage;
	float cloud_type;
	float percipitation;
	vec3 wind_direciton;
	float cloud_speed;
};


uniform float dt;
uniform float slice = 0;
uniform int numSlices;
uniform Weather weather;
uniform vec2 cloudMinMax;
uniform vec3 camPos;
uniform vec3 stepSize;
uniform vec3 texMin = vec3(-0.5);
uniform vec3 texMax = vec3(0.5);

in ncl_PerVertex{
	smooth vec2 texCoord;
};

out vec4 fragcol;

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
	base_cloud = remap(base_cloud, 0.85, 1.0, 0., 1.0); // fake cloud coverage

	float density_height_grad = densityHeightGradientForPoint(p, weather);

	base_cloud *= density_height_grad;

	return base_cloud;
	//return perlinWorley;
}

void main(){
	vec2 uv = texCoord;
	uv  -= 0.02 * dt;

	float z = slice/numSlices;
	//z = clamp(z, 0, 1);
	vec3 pos = vec3(uv, z);

	float perlinWorley = texture(cloudNoiseLowFreq, vec3(uv * 0.5, z)).x;
	vec3 worley = texture(cloudNoiseLowFreq, pos).yzw;

	float wfbm = dot(worley, vec3(0.625, 0.125, 0.25));

    // cloud shape modeled after the GPU Pro 7 chapter
    float cloud = remap(perlinWorley, wfbm - 1.0, 1.0, 0.0, 1.0);
    cloud = remap(cloud, 0.85, 1.0, 0., 1.0); // fake cloud coverage


	vec2 st = texCoord;
	st.x *= 5.0;

    vec3 col = vec3(0);
	if (st.x < 1.0)
        col += perlinWorley;
    else if(st.x < 2.)
        col += worley.x;
    else if(st.x < 3.)
        col += worley.y;
	else if(st.x < 4.)
        col += worley.z;
    else if(st.x < 5.)
        col += cloud;

	bvec3 isBlack = equal(col, vec3(0));
//	if(all(isBlack)) col = vec3(1, 0, 0);
            
    // column dividers
    float div = smoothstep(.01, 0., abs(st.x - 1.));
    div += smoothstep(.01, 0., abs(st.x - 2.));
	div += smoothstep(.01, 0., abs(st.x - 3.));
    div += smoothstep(.01, 0., abs(st.x - 4.));
        
    col = mix(col, vec3(0., 0., .866), div);

    fragcol = vec4(col, 1.0);
//	float cloud_density = sampleCloudDensity(pos, weather);
	
//    fragcol = vec4(vec3(cloud_density), 0.8);
    
//    col = vec3(gl_FragCoord.xy/vec2(1024, 720), 0);
 //   fragcol = vec4(texture(noise, pos).rgb, 1);
}