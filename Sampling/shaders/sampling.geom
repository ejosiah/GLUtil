#version 450 core
#pragma debug(on)
#pragma optimize(off)

#extension GL_NV_gpu_shader5 : enable

#define TWO_PI 6.283185307179586476925286766559

layout (points) in;
layout (line_strip, max_vertices=128) out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float time;

// gl_Position = P * V * M * vec4(position, 1);

vec2 hash22(vec2 p){
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);

}

vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yzz)*p3.zyx);
}


smooth out vec4 fColor;

void main(){
	vec2 seed = gl_in[0].gl_Position.xy;
	float f = TWO_PI/128;
	float theta = 0;
	for(int i = 0; i < 128; i++, theta += f){
		vec2 p = seed - vec2(0.002 * cos(theta), 0.002 * sin(theta));
		gl_Position = P * V * M * vec4(p, 0, 1);
		fColor = vec4(hash32(seed), 1);
		EmitVertex();
	}
	EndPrimitive();
}