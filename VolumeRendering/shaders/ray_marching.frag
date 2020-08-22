#version 450 core 

#pragma compile(on)

layout(binding = 0 ) uniform sampler3D volume;
uniform vec3 camPos;
uniform vec3 stepSize;

const int MAX_SAMPLES = 300;
const vec3 texMin = vec3(0);
const vec3 texMax = vec3(1);

in ncl_PerVertex{
	smooth vec3 pos;
	smooth vec3 uv;
};

layout(location = 0) out vec4 fragColor;

void main(){
	vec3 dataPos = pos + 0.5; // from [-0.5, 0.5] to [0, 1]
	vec3 geomDir = normalize(pos - camPos);

	vec3 dirStep = geomDir * stepSize;

	bool stop = false;

	for(int i = 0; i < MAX_SAMPLES; i++){
		dataPos += dirStep;

		stop = dot(sign(dataPos - texMin), sign(texMax - dataPos)) < 3;

		if(stop) break;

		float voxel = texture(volume, dataPos).r;

		float prev_alpha = voxel - (voxel * fragColor.a);
		fragColor.rgb = prev_alpha * vec3(voxel) + fragColor.rgb;
		fragColor.a += prev_alpha;

		if(fragColor.a > 0.99) break; 
	}
	if(fragColor.r == 0.0) discard;

}