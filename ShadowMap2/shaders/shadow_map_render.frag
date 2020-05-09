#version 450 core 

layout(binding = 0) uniform sampler2D shadowMap;

smooth in vec2 texCoord;
out vec4 fragColor;

void main(){
	vec3 depth = texture(shadowMap, texCoord).rrr;
	fragColor = vec4(depth, 1.0);
}