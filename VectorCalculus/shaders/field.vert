#version 450 core 

layout(location = 0) in vec3 position;
layout(location = 5) in vec2 uv;
layout(location=8) in mat4 xform;

smooth out vec2 v_st;

void main(){
	v_st = uv;
	gl_Position =  xform * vec4(position, 1);
}
