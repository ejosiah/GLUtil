#version 450 core 

layout(location = 0) in vec3 position;
layout(location=8) in mat4 xform;

uniform mat4 M;
uniform mat4 lightSpaceView; 

void main(){
	gl_Position = lightSpaceView * M * xform * vec4(position, 1);
}