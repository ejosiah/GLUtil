#version 450 core

in vec2 texCoord;

layout(binding = 0) uniform sampler2D shadowMap;

uniform float near_plane;
uniform float far_plane;
uniform bool perspective;

out vec4 fragColor;

float LinearizeDepth(float depth){
	    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main(){
	float depthValue = texture(shadowMap, texCoord).r;
	depthValue = perspective ? LinearizeDepth(depthValue) : depthValue;

	fragColor = vec4(vec3(depthValue), 1);
}