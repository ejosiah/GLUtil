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

float remap(float x, float a, float b, float c, float d){
	return (((x - a) / (b - a)) * (d - c)) + c;
}

vec3 remap(vec3 x, vec3 a, vec3 b, vec3 c, vec3 d){
	return (((x - a) / (b - a)) * (d - c)) + c;
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
    cloud = remap(cloud, 0.85, 1.0, 0., 1.0) * 0.85; // fake cloud coverage


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

}