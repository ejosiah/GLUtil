#version 450 core
#pragma debug(on)
#pragma optimize(off)


layout(location=0) in vec3 position;
smooth out vec3 texCoord;

uniform mat4 MVP;

void main(){
	texCoord = position;
	vec4 pos = MVP * vec4(position, 1.0);
	gl_Position = pos.xyww;
}
