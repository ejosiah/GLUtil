#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=0) uniform sampler3D noise;

smooth in vec3 texPos;

out vec4 fColor;

void main(){
	fColor = texture(noise, texPos);
}