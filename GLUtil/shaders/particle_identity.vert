#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP;

struct Particle{
	vec3 position;
	float damping;

	vec3 velocity;
	float inverseMass;

	vec3 acceleration;
	int forceGenerators;

	vec3 forceAccum;
	int _active;

	float lifetime;
};

layout(std430, binding=0) buffer PARTICLE_SSBO{
	Particle particles[];
};

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;
layout(location=6) in vec2 uv1;
layout(location=8) in mat4 xform;

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord[8];
	smooth vec4 color;
} vertex;

layout(xfb_buffer=0, xfb_offset=0) out vec3 capture_position;

mat4 translate(vec3 dv){
    return mat4(
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        dv	   , 1);
}

uniform int offset = -1;

void main(){
	Particle particle = particles[gl_InstanceID + offset];
	mat4 particlexForm = offset < 0 ? mat4(1) : translate(particle.position);
	mat4 model =   M * particlexForm * xform;

	mat4 MV = V *  model;
	mat3 NM = transpose(inverse(mat3(MV)));
	vertex.normal = normalize(NM * normal);
	vertex.position = (MV * vec4(position, 1)).xyz;
	vertex.texCoord[0] = uv;
	vertex.texCoord[1] = uv1;
	vertex.color = color;

	capture_position = (M * xform * vec4(position, 1.0)).xyz;

	gl_Position = P * V * model * vec4(position, 1);
}