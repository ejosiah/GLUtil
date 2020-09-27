#version 450 core

smooth in vec2 uv;
smooth in float val;

uniform float minVal;
uniform float maxVal;

layout(location = 0) out vec4 fragColor;

void main(){
	float ratio = (val - minVal)/( maxVal - minVal);
	fragColor.r = smoothstep(0.6f, 0.8f, ratio);
	fragColor.g = smoothstep(0.0f, 0.4f, ratio) - smoothstep(0.8f, 1.0f, ratio);
	fragColor.b = 1 - smoothstep(0.4f, 0.6f, ratio);
	fragColor.a = 1;
}