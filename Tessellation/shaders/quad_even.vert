#version 460 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position; // TODO change to vec4

void main(){
	gl_Position = vec4(position, 1.0);
}
