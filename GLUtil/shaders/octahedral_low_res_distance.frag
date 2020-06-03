#version 450 core

#pragma storeIntermediate(on)

const float PI = 3.1415926535897932384626422832795028841971;
const float TWO_PI = 6.2831853071795864769252867665590057683943;

//layout(binding = 0) uniform samplerCube distanceMap;
layout(binding = 0) uniform sampler2DArray distanceMap;

smooth in vec2 texCoord;
uniform int layer;

layout(location = 0) out vec4 fragColor;

vec3 octDecode(in vec2 v);


void main(){
 
//	vec3 direction = octDecode(texCoord * vec2(2.0) - vec2(1.0));
//	fragColor = texture(distanceMap, direction);
	fragColor = texture(distanceMap, vec3(texCoord, layer));
}

#pragma include("octahedral.glsl")