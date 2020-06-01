#version 450 core

layout(binding = 0 ) uniform sampler2DArray scene;

in ncl_PerVertex{
	smooth vec3 texCoord;
};


uniform int numLayers;
uniform int layer;
uniform bool isDistance = false;
uniform bool isDistanceSqrd = false;

vec2 octEncode(in vec3 v);

out vec4 fragColor;

vec3 getColor(float layer){
	vec2 uv = octEncode(normalize(texCoord)) * 0.5 + 0.5;
	vec3 ptr = vec3(uv, layer);
	if(isDistance){
		return texture(scene, ptr).rrr;
	}else if(isDistanceSqrd){
		return texture(scene, ptr).ggg;
	}else{
		return texture(scene, ptr).rgb;
	}
}


void main(){
	float actual_layer = max(0, min(numLayers - 1 , floor(layer + 0.5)));

	vec3 color = getColor(actual_layer);
	color = isDistance || isDistanceSqrd ? color / (color + vec3(1.0)) : color;
	fragColor = vec4(color, 1.0);
}

#pragma include("octahedral.glsl")