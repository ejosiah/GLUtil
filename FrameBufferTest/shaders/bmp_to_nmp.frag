#version 450 core 

layout(binding = 0) uniform sampler2D bumpMap;

in ncl_PerVertex{
	smooth vec2 texCoord;
};

uniform bool flip = true;

out vec4 fragColor;

vec3 packNormal(vec3 N)
{
	return N * vec3(0.5) + vec3(0.5);
}

float h(float u, float v){
	return texture(bumpMap, vec2(u, v)).r;
}


void main(){
	vec3 color = texture(bumpMap,texCoord).rgb;

	vec3 normal = packNormal(normalize(vec3(-dFdx(color.x), -dFdy(color.x), 1)));
	fragColor = vec4(flip ? color : normal, 1);
}