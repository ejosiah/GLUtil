#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 lightUV;

smooth out vec2 interpolatedTexCoord;
smooth out vec2 interpolatedLightTexCoord;

void main(){
	interpolatedTexCoord = uv;
	interpolatedLightTexCoord = lightUV;
	gl_Position = P * V * M * vec4(position, 1);
}