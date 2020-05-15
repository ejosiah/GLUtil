struct LightPart {
	vec3 diffuse;
	vec3 specular;
};

struct Lighting {
	LightPart direct;
	LightPart indirect;
};

struct Surface {
	vec3 position;
	vec3 normal;
	vec3 albedo;
	float metallic;
	float roughness;
};

struct Scene {
	samplerCube irradianceMap;
	samplerCube prefilterMap;
	sampler2D brdfLUT;
	vec3 camPos;
};