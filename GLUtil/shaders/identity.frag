#version 450 core
#pragma debug(on)
#pragma optimize(off)

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

out vec4 fragColor;

void main(){
	fragColor = vec4(0);
}