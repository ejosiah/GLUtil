#version 450 core
#pragma debug(on)
#pragma optimize(off)

smooth in vec4 fColor;
out vec4 fragColor;

void main(){
	fragColor =  fColor;
}