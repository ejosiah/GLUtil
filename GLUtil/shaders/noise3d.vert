#version 450 core
#pragma debug(on)
#pragma optimize(off)
#pragma nocomp

#pragma include("simple_light.vert")

uniform mat4 V;
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform float scale = 0.2;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;


smooth out vec3 texPos;
smooth out vec3 norm;

void main(){
	texPos = position * scale;
	norm = normal;
	doLight(MV * vec4(position, 1.0), V);
	gl_Position = MVP * vec4(position, 1);
}