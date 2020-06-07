#version 450 core
layout(binding = 0) uniform sampler2DArray map;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

uniform int layer;

layout(location = 0) out vec4 fragColor;

void main(){
	fragColor = texture(map, vec3(texCoord, layer));
}