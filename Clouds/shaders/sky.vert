#version 450 core 

uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=8) in mat4 xform;

uniform vec3 camPos;
uniform vec3 sunPos;

out ncl_PerVertex{
	smooth vec3 sunDir;
	smooth vec3 norm;
};

void main(){
	
	sunDir = sunPos - camPos;
	norm = normal;

	gl_Position = MVP * xform * vec4(position, 1);
}
