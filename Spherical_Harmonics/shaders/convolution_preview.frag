#version 450 core 

layout(binding=0) uniform samplerCube irradianceMap;
layout(binding=1) uniform sampler2D map;
smooth in vec3 texCoord;
smooth in vec2 st;
out vec4 fragColor;

vec2 spherical(vec3 v){
	float r = length(v);
	return vec2(acos(v.z / r), atan(v.x / v.y));
}

const vec2 invAtan = vec2(0.1591, 0.3183);
const float PI = 22.0/7;
const float TWO_PI = 2 * PI;
//vec2 spherical(vec3 v){
//	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
//    uv *= invAtan;
//    uv += 0.5;
//    return uv;
//}

float signNotZero(float f){
  return(f >= 0.0) ? 1.0 : -1.0;
}
vec2 signNotZero(vec2 v) {
  return vec2(signNotZero(v.x), signNotZero(v.y));
}

/** Assumes that v is a unit vector. The result is an octahedral vector on the [-1, +1] square. */
vec2 octEncode(in vec3 v) {
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    vec2 result = v.xy * (1.0 / l1norm);
    if (v.z < 0.0) {
        result = (1.0 - abs(result.yx)) * signNotZero(result.xy);
    }
    return result;
}


/** Returns a unit vector. Argument o is an octahedral vector packed via octEncode,
    on the [-1, +1] square*/
vec3 octDecode(vec2 o) {
    vec3 v = vec3(o.x, o.y, 1.0 - abs(o.x) - abs(o.y));
    if (v.z < 0.0) {
        v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);
    }
    return normalize(v);
}


vec3 direction_from_spherical(vec2 uv)
{
	float phi = uv.x * TWO_PI;
	float theta = uv.y * PI;

	return vec3(
		sin(phi) * sin(theta) * -1.0,
		cos(theta) * -1.0,
		cos(phi) * sin(theta) * -1.0
	);
}

void main(){
	//vec3 idx = direction_from_spherical(st);
//	vec3 idx = octDecode(st * vec2(2.0) - vec2(1.0));
	vec3 color = texture(irradianceMap, texCoord).rgb;
	//color = texture(map, spherical(texCoord)).rgb;
	fragColor = vec4(color, 1);
}