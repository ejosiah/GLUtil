#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform vec4 color;

out vec4 fragColor;

void main(){
	fragColor = color;
}