#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265

layout(binding=0) uniform sampler2D noise;

uniform vec4 sky = vec4(0.3, 0.3, 0.9, 1.0);
uniform vec4 cloud = vec4(1.0);

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

out vec4 color;

void main(){
	vec4 oct = texture(noise, vertex.texCoord);
	float sum =  ( oct.r + oct.g + oct.b + oct.a - 1)/2;
	float t = (cos(sum * PI) + 1.0)/2.0;
	vec4 c = mix(sky, cloud, t);
	color = vec4(c.rgb, 1.0);
}