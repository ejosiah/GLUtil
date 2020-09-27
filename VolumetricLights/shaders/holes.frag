#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout(std430, binding=3) buffer HOLES_SSOB{
	vec4 holes[];
};

uniform int num_holes = 100;
uniform vec3 light_color;

in ncl_PerVertex {
	smooth vec3 local_pos;
};


out vec4 fragColor;

uniform vec4 hole = vec4(0.565685451, 0.565685451, 0.600000024, 0.2);
uniform vec3 cam_pos;

void main(){
	vec3 pos = local_pos;
	float v = 1;
	fragColor = gl_FrontFacing ? vec4(0) : vec4(light_color, 1);
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