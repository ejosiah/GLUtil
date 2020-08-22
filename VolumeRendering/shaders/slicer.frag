#version 450 core 

layout(binding = 0) uniform sampler3D volume;

layout(std430, binding = 0 ) buffer SLICES{
    int slices[];
};


in smooth vec3 vUV;
int layer;

out vec4 fragColor;

void main(){
	//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
	//Note that since at the time of texture creation, we gave the internal format as GL_RED
	//we can get the sample value from the texture using the red channel. Here, we set all 4
	//components as the sample value in the texture which gives us a shader of grey.
	float value = texture(volume, vUV).r;

	slices[gl_Layer] = gl_Layer;

	if(value < 0.1) discard;
	fragColor = vec4(value);
}
