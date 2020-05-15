#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=0) uniform samplerCube probeMap;

const float MAX_REFLECTION_LOD = 4.0;

smooth in vec3 texCoord;
out vec4 fragColor;

uniform float roughness;
uniform bool useRoughness = false;

void main(){
	vec3 color;

	if(useRoughness){
		color = textureLod(probeMap, texCoord, roughness * MAX_REFLECTION_LOD).rgb;
	}else{
		color = texture(probeMap, texCoord).rgb;
	}

	fragColor = vec4(color, 1.0);
}