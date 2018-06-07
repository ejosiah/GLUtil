#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location=0) in vec3 position;
layout(location=4) in vec4 color;


layout(binding=0) uniform samplerBuffer faces_tbo;

smooth out vec4 vcolor;

mat4 model(){
	return mat4(
		texelFetch(faces_tbo, gl_InstanceID * 4),
		texelFetch(faces_tbo, gl_InstanceID * 4 + 1),
		texelFetch(faces_tbo, gl_InstanceID * 4 + 2),
		texelFetch(faces_tbo, gl_InstanceID * 4 + 3)
	);
	return mat4(1);
}


void main(){
	vcolor = color;
	gl_Position = model() * vec4(position, 1);
}