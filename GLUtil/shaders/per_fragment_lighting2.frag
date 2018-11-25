#version 450 core
#pragma debug(on)
#pragma optimize(off)
#pragma storeIntermediate(on)
#pragma include("lighting.frag.glsl")
#pragma include("composite_diffuse.glsl")

out vec4 fragColor;

uniform bool gammaCorrect;

const float gamma = 2.2;

void main(){
	vec4 color = phongLightModel(mat4(1));
	fragColor = gammaCorrect ? vec4(pow(color.rgb, vec3(1/gamma)), 1.0) : color;	
}
