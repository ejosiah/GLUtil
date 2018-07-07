#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(location = 0) in vec4 position;

layout(binding=1) uniform samplerBuffer testData;

out vec3 test_output;

void main(){
	gl_Position = position;
	test_output = vec3(0);
}