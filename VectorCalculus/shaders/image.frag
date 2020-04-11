#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding = 0) uniform sampler2D image0;
layout(binding = 1) uniform sampler2D image1;

uniform int id;

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

out vec4 fragColor;

void main(){
	if(id == 1){
		fragColor = texture(image0, vertex.texCoord);
	}else if(id == 2){
		fragColor = texture(image1, vertex.texCoord);
	}else{
		fragColor = vertex.color;
	}
}
