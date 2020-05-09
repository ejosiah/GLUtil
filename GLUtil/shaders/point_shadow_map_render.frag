#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=0) uniform samplerCube shadowMap;
smooth in vec3 texCoord;
out vec4 fragColor;

void main(){
	float depth = texture(shadowMap, texCoord).r;

	fragColor = vec4(vec3(depth), 1);
}