#version 450 core 

layout(binding = 0) uniform sampler2DArray image;


uniform int numLayers;
uniform bool isDistance = false;
uniform bool isDistanceSqrd = false;

in ncl_PerVertex{
	smooth vec2 texCoord;
		flat int layer;
};

out vec4 fragColor;

vec3 getColor(int layer){
	if(isDistance){
		return texture(image, vec3(texCoord, layer)).rrr;
	}else if(isDistanceSqrd){
		return texture(image, vec3(texCoord, layer)).ggg;
	}else{
		return texture(image, vec3(texCoord, layer)).rgb;
	}
}

void main(){
	float actual_layer = max(0, min(numLayers - 1 , floor(layer + 0.5)));
	vec3 color = getColor(int(actual_layer));
	color = isDistance || isDistanceSqrd ? color / (color + vec3(1.0)) : color;
	fragColor = vec4(color, 1.0);
}
