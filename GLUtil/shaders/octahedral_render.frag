#version 450 core 

layout(binding = 0) uniform sampler2DArray image;


uniform float numLayers;
uniform float layer = 0;

smooth in vec2 texCoord;

out vec4 fragColor;

void main(){
	float actual_layer = max(0, min(numLayers - 1 , floor(layer + 0.5)));
	fragColor = vec4(texture(image, vec3(texCoord, actual_layer)).rgb, 1);
}