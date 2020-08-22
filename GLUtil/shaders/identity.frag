#version 450 core
#pragma debug(on)
#pragma optimize(off)

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;

vec4 Checkerboard(vec2 uv)
{
	vec3 color = vec3(mod(floor(uv.x * 4.0) + floor(uv.y * 4.0), 2.0) < 1.0 ? 1.0 : 0.4);
	return vec4(color, 1.0);
}

uniform bool checker = false;
uniform int tid = 0;

out vec4 fragColor;

void main(){
	fragColor = checker ? Checkerboard(vertex.texCoord[tid]) : vertex.color;
}