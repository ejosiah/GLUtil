struct ToonShader{
	int levels;
	float scaleFactor;
} toonShader;

vec4 diffuseContrib(vec3 L, vec3 N, LightSource light, Material m){
	toonShader.levels = 3;
	toonShader.scaleFactor = 1.0 / toonShader.levels;
	return floor(max(dot(l, n), 0) * toonShader.levels) * toonShader.scaleFactor * light.diffuse *  getDiffuse(m);
}