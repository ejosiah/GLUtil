#version 450 core 

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 texCoord;
layout(location=6) in vec2 uv1;
layout(location=8) in mat4 xform;



out ncl_PerVertex{
	flat int instanceId;
	smooth vec2 uv;
};

void main(){
	uv = texCoord;
	instanceId = gl_InstanceID;
	gl_Position =  xform * vec4(position, 1);
}