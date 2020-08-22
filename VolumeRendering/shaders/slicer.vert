#version 450 core 

layout(location = 0 ) in vec3 position;

out	int layer;

void main(){
	layer = gl_InstanceID;
	gl_Position = vec4(position, 1);

}