#version 450 core

layout(binding=0) uniform samplerCubeArray cubeArrayMap;

smooth in vec3 texCoord;

uniform int layer = 0;
uniform bool isDistance = false;
uniform bool isDistanceSqure = false;

out vec4 fragColor;

vec3 color(){
	vec4 dir = vec4(texCoord, layer);
	vec3 c = vec3(0);
	if(isDistance){
		c = texture(cubeArrayMap, dir).rrr;
	}else if(isDistanceSqure){
		c = texture(cubeArrayMap, dir).ggg;
	}else{
		c = texture(cubeArrayMap, dir).rgb;
	}

	if(isDistance || isDistanceSqure){
		c = c/(c + 1);
	}
	return c;
}

void main(){
	fragColor = vec4(color(), 1.0);
}