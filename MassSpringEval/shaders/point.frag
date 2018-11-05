#version 450 core
#pragma debug(on)
#pragma optimize(off)

//layout(binding = 0) uniform sampler2D image0;

smooth in vec4 fColor;
out vec4 fragColor;

void main(){
	fragColor = fColor;
}