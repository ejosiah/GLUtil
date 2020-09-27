#version 450 core

smooth in vec2 uv;
smooth in float val;
smooth in vec3 normal;

uniform float minVal;
uniform float maxVal;

layout(location = 0) out vec4 fragColor;

void main(){
	vec3 N = normalize(normal);
	vec3 L = vec3(0, 0, 1);

	float ratio = (val - minVal)/( maxVal - minVal);
	vec3 col = vec3(0);
	col.r = smoothstep(0.6f, 0.8f, ratio);
	col.g = smoothstep(0.0f, 0.4f, ratio) - smoothstep(0.8f, 1.0f, ratio);
	col.b = 1 - smoothstep(0.4f, 0.6f, ratio);


	fragColor.rgb = col;
	fragColor.a = 1;
}