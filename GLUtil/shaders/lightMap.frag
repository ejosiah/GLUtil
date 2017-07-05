#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=0) uniform sampler2D image0;
layout(binding=1) uniform sampler2D image1;

smooth in vec2 interpolatedTexCoord;
smooth in vec2 interpolatedLightTexCoord;

out vec4 fragColor;

void main(){
	vec4 color = texture(image0, interpolatedTexCoord);
	vec4 light = texture(image1, interpolatedLightTexCoord);
	fragColor = light * color;
}