#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 lightUV;


smooth out vec3 interpolatedPosition;
smooth out vec3 interpolatedNormal;
smooth out vec2 interpolatedTexCoord;
smooth out vec2 interpolatedLightTexCoord;
smooth out vec4 interpolatedColor;

void main(){
	interpolatedNormal = normalize(normalMatrix * normal);
	interpolatedPosition = (MV * vec4(position, 1)).xyz;
	interpolatedTexCoord = uv;
	interpolatedLightTexCoord = lightUV;
	interpolatedColor = color;
	gl_Position = MVP * vec4(position, 1);
}