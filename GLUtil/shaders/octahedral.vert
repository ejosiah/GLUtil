#version 450 core 


layout(location = 0) in vec3 position;
layout(location = 5) in vec2 uv;
layout(location=8) in mat4 xform;

out ncl_PerVertex{
	smooth vec2 texCoord;
};

void main(){
	texCoord = uv;
	gl_Position =  xform * vec4(position, 1);
}