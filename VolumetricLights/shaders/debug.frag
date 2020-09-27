#version 450 core

layout(binding = 0) uniform samplerCube shadowMap;

smooth in vec3 world_pos;

uniform vec3 light_pos;

out vec4 fragColor;

void main(){
	vec3 frag_to_light = world_pos - light_pos;

	fragColor.a = 1;
	fragColor.rgb = vec3(1, 0, 0) * (1 - texture(shadowMap, frag_to_light).r);
}