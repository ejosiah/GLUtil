#version 450 core
#pragma storeIntermediate(on)

layout(binding = 0) uniform samplerCube cubeMap;

const float PI = 3.1415926535897932384626422832795028841971;

uniform int numSamples;
uniform float lobeSize;

vec3 octDecode(vec2 uv);
vec3 hammersleySphere(int i, uint N);

uniform bool irradiance = false;

in smooth vec3 texCoord;

layout(location=0) out vec4 fragColor;

void main(){
	
	vec3 N = normalize(texCoord);
	vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);

	vec3 total = vec3(0);
	for(int i = 0; i < numSamples; i++){
		vec3 p = hammersleySphere(i, numSamples);
		vec3 sampleDirection = (p.x * right + p.y * up + p.z * N) * lobeSize;

		total += texture(cubeMap, sampleDirection).rgb;
	}

	total /= float(numSamples);
	//total = irradiance ? total * PI : total;

	fragColor = vec4(total, 1.0);
}

#pragma include("octahedral.glsl")
#pragma include("sampling.glsl")