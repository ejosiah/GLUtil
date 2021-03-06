#version 450 core

#pragma include("LightFieldProbe.glsl")
#pragma include("octahedral.glsl")

in VERTEX{
	smooth vec3 position;
	smooth vec4 lightSpacePos;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
	smooth vec3 bitangent;
	smooth vec2 uv;
} vert_in;

uniform vec3 camPos;

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

uniform int layer = 0;

//in ncl_PerVertex{
//	smooth vec2 texCoord;
//};
//
out vec4 fragColor;

void main(){
	vec3 worldPos = vert_in.position;
	vec3 N = normalize(vert_in.normal);
	vec3 V = normalize(camPos - worldPos);

	vec3 wi = normalize(reflect(-V, N));
	vec2 texCoord = octEncode(wi);

	vec3 color = texture(lightFieldSurface.radianceProbeGrid.sampler, vec3(texCoord, layer)).rgb;
//	vec3 color = (texture(lightFieldSurface.normalProbeGrid.sampler, vec3(texCoord, layer)) * lightFieldSurface.normalProbeGrid.readMultiplyFirst + lightFieldSurface.normalProbeGrid.readAddSecond).rgb;
//	vec3 color = texture(lightFieldSurface.distanceProbeGrid.sampler, vec3(texCoord, layer)).rrr;
//	vec3 color = texture(lightFieldSurface.lowResolutionDistanceProbeGrid.sampler, vec3(texCoord, layer)).rrr;
//	vec3 dir = direction_from_spherical(texCoord);
//	vec3 dir = octDecode(texCoord * vec2(2.0) - vec2(1.0));
//	vec3 color = texture(lightFieldSurface.irradianceProbeGrid.sampler, vec4(dir, layer)).rgb;
//	vec3 color = texture(lightFieldSurface.meanDistProbeGrid.sampler, vec4(dir, layer)).rrr;
//	color /= (color + 1);
	fragColor = vec4(color, 1);
}