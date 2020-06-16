#version 450 core


in ncl_PerVetex {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 texCoord;
	smooth vec4 color;
} ncl_in;

out vec4 fragColor;

void main(){
	fragColor = vec4(1, 0, 0, 1);
}