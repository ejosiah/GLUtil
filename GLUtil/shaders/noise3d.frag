#version 450 core
#pragma debug(on)
#pragma optimize(off)
#pragma nocomp

#pragma include("simple_light.frag")

#define PI 3.14159265

layout(binding=0) uniform sampler3D noise;

uniform vec4 sky = vec4(0.3, 0.3, 0.9, 1.0);
uniform vec4 cloud = vec4(1.0);

smooth in vec3 texPos;
smooth in vec3 norm;

out vec4 fColor;

void main(){
	vec4 oct = texture(noise, texPos);
	float sum =  ( oct.r + oct.g + oct.b + oct.a - 1)/2;
	float t = (cos(sum * PI) + 1.0)/2.0;
	vec4 color = vec4(mix(sky, cloud, t).rgb, 1.0);
	fColor = applyLight(norm, color, 0.5);
}