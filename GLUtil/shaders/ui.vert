#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 MVP;

layout(location=0) in vec3 position;

void main(){
	gl_Position = MVP * vec4(position, 1);
}
