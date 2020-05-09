#version 450 core

layout(location = 0) in vec3 position;
layout(location = 5) in vec2 uv;

smooth out vec2 texCoord;

void main(){
	texCoord = uv;
	gl_Position = vec4(position, 1.0);
}