#version 450 core 


layout(binding=0, offset=0) uniform atomic_uint next_index;

layout(std430, binding = 0) buffer DEPTH_CAPTURE{
	float depth_values[];
};

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;

out vec4 fragColor;

void main(){
	atomicCounterIncrement(next_index);
	int index = int(atomicCounter(next_index));
	depth_values[index] = index;
	fragColor = vec4(vec3(gl_FragCoord.z), 1);
}

