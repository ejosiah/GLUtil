#version 450 core
#pragma debug(on)
#pragma optimize(off)


layout(location=0) in vec3 position;
smooth out vec3 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	texCoord = position;
	mat4 rotV = mat4(mat3(V));
	vec4 pos = P * rotV * M * vec4(position, 1.0);
	gl_Position = pos.xyww;
}
