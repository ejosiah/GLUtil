#version 450 core 

layout(binding = 0) uniform sampler2D bumpMap;

#define DELTA 0.000001

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

vec3 calcuateNormal(){
	float u = texCoord.s;
	float v = texCoord.t;
	
	float strength = float(textureSize(bumpMap, 0).y)/16;

	float tl = abs(h(u - 1, v - 1));
	float l = abs(h(u = 1, v));
	float bl = abs(h(u - 1, v + 1));
	float b = abs(h(u, v + 1));
	float br = abs(h(u + 1, v + 1));
	float r = abs(h(u + 1, v));
	float tr = abs(h(u + 1, v - 1));
	float t = abs(h(u, v - 1));

	float dx = tr + 2 * r + br - tl - 2 * l - bl;
	float dy = bl + 2 * b + br - tl - 2 * t - tr;
	vec3 n = vec3(dx, dy, 2);
	n = normalize(n);

	return n;
}

void main(){
	vec3 color = texture(bumpMap,texCoord).rgb;

	vec3 normal = packNormal(normalize(vec3(-dFdx(color.x), -dFdy(color.y), 5)));
//	vec3 normal = packNormal(calcuateNormal());
	fragColor = vec4(flip ? color : normal, 1);
}