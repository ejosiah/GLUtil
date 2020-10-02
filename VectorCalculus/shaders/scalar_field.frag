#version 450 core
#pragma compile(off)

uniform float minVal;
uniform float maxVal;

in ncl_PerVertex{
	smooth vec2 uv;
	smooth float val;
	smooth vec3 normal;
};


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