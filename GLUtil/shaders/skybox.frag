#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=7) uniform samplerCube skybox;
smooth in vec3 texCoord;
out vec4 fragColor;

void main(){
	fragColor = texture(skybox, normalize(texCoord));
}