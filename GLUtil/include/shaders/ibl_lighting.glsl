#pragma include("constants.glsl")
#pragma include("math.glsl")
#pragma include("pbr_lights.glsl")


void imageBasedLighting(Scene scene, Surface surface, inout Lighting lighting) {

	vec3 N = surface.N;
	lighting.indirect.diffuse = texture(scene.irradianceMap, N).rgb;

	vec3 V = normalize(scene.camPos - surface.position);
	vec3 R = reflect(-V, N);
	float roughness = surface.roughness;

	lighting.indirect.specular = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
}