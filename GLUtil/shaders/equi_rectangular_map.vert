#version 450 core

uniform mat4 MVP;

layout(location=0) in vec3 position;

smooth out vec3 localPos;

void main(){
	localPos = position;
	gl_Position = MVP * vec4(localPos, 1.0);
}
