#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(std430, binding=0) buffer HOLES_SSOB{
	vec4 holes[];
};

uniform int num_holes = 100;

in ncl_PerVertex {
	smooth vec3 local_pos;
};


out vec4 fragColor;

uniform vec4 hole = vec4(0.565685451, 0.565685451, 0.600000024, 0.2);

void main(){
	vec3 pos = local_pos;
	float v = 1;
	fragColor = gl_FrontFacing ? vec4(v, 0, 0, 1) : vec4(v * 0.3, 0, 0, 1);
	for(int i = 0; i < num_holes; i++){
		vec3 center = holes[i].xyz;
		float radius = holes[i].w;
		vec3 d = pos - center;
		if(dot(d, d) < radius * radius){
			discard;
			break;
		}
	}
	
}