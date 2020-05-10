#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(binding=0) uniform samplerCube skybox;
smooth in vec3 textCoord_out;
out vec4 fragColor;

void main(){
	vec3 color = texture(skybox, textCoord_out).xyz;

	color /= color + vec3(1.0);
	color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0);
}