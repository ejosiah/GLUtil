#version 450 core 


layout(binding = 0) uniform sampler2D image;

smooth in vec2 texCoord;

out vec4 fragColor;

void main(){
	fragColor = vec4(texture(image, texCoord).rgb, 1);
}