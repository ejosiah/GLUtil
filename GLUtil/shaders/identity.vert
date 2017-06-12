#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

void main(){
	vertex.normal = normalize(normalMatrix * normal);
	vertex.position = (MV * vec4(position, 1)).xyz;
	vertex.texCoord = uv;
	vertex.color = color;
	gl_Position = MVP * vec4(position, 1);
}