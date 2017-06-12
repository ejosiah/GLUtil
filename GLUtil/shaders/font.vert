#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec4 coord;

uniform mat4 P;
smooth out vec2 texCoord;

void main(){
	gl_Position = P * vec4(coord.xy, 0, 1);
	texCoord = coord.zw;
}