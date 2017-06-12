#version 450 core
#pragma debug(on)
#pragma optimize(off)

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec4 lightDirection;
} in_vertex;
noperspective in vec3 edgeDistance;


out vec4 fragColor;

void main(){
	fragColor = in_vertex.color;
}