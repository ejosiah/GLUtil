#version 450 core  
#pragma compile(off)

layout(std430, binding=3) buffer HOLES_SSOB{
	vec4 holes[];
};

uniform int num_holes = 100;

void onFragment(vec3 localPos, vec3 worldPos, vec3 viewPos){
	vec3 pos = localPos;
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