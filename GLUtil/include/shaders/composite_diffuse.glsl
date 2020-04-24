struct ToonShader{
	int levels;
	float scaleFactor;
} toonShader;

uniform int diffuseType = 0;

vec4 asdDiffuse(vec3 L, vec3 N, LightSource light, Material m, vec2 uv){
	return  max(dot(L, N), 0)  * light.diffuse * getDiffuse(m, uv);
}

vec4 toonDiffuse(vec3 L, vec3 N, LightSource light, Material m, vec2 uv){
	toonShader.levels = 3;
	toonShader.scaleFactor = 1.0 / toonShader.levels;
	return floor(max(dot(L, N), 0) * toonShader.levels) * toonShader.scaleFactor * light.diffuse *  getDiffuse(m, uv);
}

vec4 hemisphereDiffuse(vec3 L, vec3 N, LightSource light, Material m, vec2 uv){
    float a = 0.5 + (0.5 * dot(L, N));
    return vec4(mix(vec3(0), vec3(1), a), 1);
}

vec4 diffuseContrib(vec3 L, vec3 N, LightSource light, Material m, vec2 uv){
	switch(diffuseType){
        case 0: return asdDiffuse(L, N, light, m, uv);
        case 1: return toonDiffuse(L, N, light, m, uv);
        case 2: return hemisphereDiffuse(L, N, light, m, uv);
        default: return vec4(0);
	}
}