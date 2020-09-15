#version 450

layout(location=0) in vec3 position; // TODO change to vec4

void main(){
	gl_Position = vec4(position, 1.0);
}