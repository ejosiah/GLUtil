#version 450 core 

layout(binding = 0) uniform sampler3D heightMap;

smooth in vec2 st;

uniform float minVal;
uniform float maxVal;
uniform int slice;
uniform int numSlices;
uniform bool isHeatMap = true;


out vec4 fragColor;

void main(){
	float u = float(slice)/float(numSlices);
	fragColor.a = 1;
	float val = texture(heightMap, vec3(st, u)).r;
	if(isHeatMap){
		float ratio = (val - minVal)/( maxVal - minVal);
		fragColor.r = smoothstep(0.6f, 0.8f, ratio);
		fragColor.g = smoothstep(0.0f, 0.4f, ratio) - smoothstep(0.8f, 1.0f, ratio);
		fragColor.b = 1 - smoothstep(0.4f, 0.6f, ratio);
	}else{
		fragColor.rgb = vec3(val);
	}
}
