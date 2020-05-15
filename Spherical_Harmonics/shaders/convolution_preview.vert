#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec3 position;
layout(location=5) in vec2 uv;
layout(location = 8) in mat4 xform;

uniform mat4 MVP;


out ncl_PerVertex{
	smooth vec3 texCoord;
	smooth vec2 st;
};

void main(){
    texCoord = position;
	st = uv;
	gl_Position = MVP * vec4(position, 1.0);
}