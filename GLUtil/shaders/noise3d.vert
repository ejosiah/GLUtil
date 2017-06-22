#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform float scale = 0.25;

layout(location=0) in vec3 position;


smooth out vec3 texPos;

void main(){
	texPos = position * scale;
	gl_Position = MVP * vec4(position, 1);
}