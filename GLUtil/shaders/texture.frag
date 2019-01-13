#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 0) uniform sampler2D image0;
layout(binding = 1) uniform sampler2D image1;

in VERTEX {
	smooth vec3 position;


	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

out vec4 fragColor;

void main(){
	vec4 color = texture(image0, vertex.texCoord);
	vec4 oldLeather = texture(image1, vertex.texCoord);
	fragColor = mix(color, oldLeather, 0.3);
	fragColor = color;
}