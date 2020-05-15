#version 450 core
#pragma debug(on)
#pragma optimize(off)


layout(location=0) in vec3 position;
smooth out vec3 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 probe_loc;

void main(){
	texCoord = position;
	vec4 worldPos = M * vec4(position, 1.0);
	mat4 rotV = mat4(mat3(V));
	vec4 pos = P * rotV * worldPos;
	gl_Position = pos;
}
