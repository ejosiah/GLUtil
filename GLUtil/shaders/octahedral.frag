#version 450 core

#pragma storeIntermediate(on)

const float PI = 3.1415926535897932384626422832795028841971;
const float TWO_PI = 6.2831853071795864769252867665590057683943;

layout(binding = 0 ) uniform samplerCube radianceMap;
layout(binding = 1 ) uniform samplerCube normalMap;
layout(binding = 2) uniform samplerCube distanceMap;

smooth in vec2 texCoord;
//smooth in vec4 o_color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec4 o_distance;

vec3 octDecode(in vec2 v);

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
	//vec3 direction = direction_from_spherical(texCoord);
	vec3 direction = octDecode(texCoord * vec2(2.0) - vec2(1.0));

	fragColor = texture(radianceMap, direction);
//	fragColor = o_color;
	o_normal = texture(normalMap, direction);
	o_distance = texture(distanceMap, direction);

}

#pragma include("octahedral.glsl")