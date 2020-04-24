#pragma include("constants.glsl")
#pragma include("lightModel.glsl")
#pragma include("vertex_in.glsl")

layout(binding = 0) uniform sampler2D ambientMap;
layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D displacementMap;
layout(binding = 5) uniform sampler2D reflectionMap;
layout(binding = 6) uniform sampler2D ambiantOcclusionMap;

uniform bool flipNormal = false;
uniform bool displace = false;
uniform float height_scale = 1.0;

vec4 getAmbience(Material m, vec2 uv);
vec4 getDiffuse(Material m, vec2 uv);
vec4 getSpecular(Material m, vec2 uv);
vec4 diffuseContrib(vec3 L, vec3 N, LightSource light, Material m, vec2 uv);
vec2 get_uv(vec3 viewDir);
vec2 parallaxMapping(vec3 viewDir);
vec2 parallaxMapping0(vec3 viewDir);

float daf(float dist, LightSource light){
	if (light.position.w == 0) return 1;
	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);
}

float saf(LightSource light, vec3 spotDirection, vec3 lightDirection, float h){
	if (light.position.w == 0) return 1;
	vec3 l = normalize(lightDirection);
	vec3 d =   normalize(spotDirection);
	
	if(light.spotAngle >= 180) 	return 1.0;
	if (light.spotAngle <= 0) return 0.0;
	
	float _LdotD = dot(-l, d);

	float inner = cos(radians(light.spotAngle - 5));
	float outter = cos(radians(light.spotAngle));
	
	float sl = smoothstep(outter, inner, _LdotD);

	return pow(sl, h); 
}


vec4 apply(LightSource light, vec3 spotDirection, vec4 direction, Material m, vec2 uv){
	if(!light.on) return vec4(0);
	vec3 n = normalize(vertex_in.normal);
	n = (!gl_FrontFacing && flipNormal) ? -n : n;
	vec3 N = lightModel.useObjectSpace ? (2.0 * texture(normalMap, vertex_in.texCoord) - 1.0).xyz : n;
	N = (!gl_FrontFacing && flipNormal) ? -N : N;
	vec3 L = normalize(direction.xyz);
	float f = m.shininess;
		
	float _daf = daf(length(L), light);

	float _saf = saf(light, spotDirection, L, light.spotExponent);
	
	vec4 ambient = light.ambient * m.ambient;
	
	vec4 diffuse =  diffuseContrib(L, N, light, m, uv);
	
	vec3 E = normalize(vertex_in.eyes);
	vec3 S = normalize(L + E);	// half way vector between light direction and eyes
	vec4 specular = pow(max(dot(S, N), 0), f) * light.specular * getSpecular(m, uv);

	return  _daf * _saf * ((ambient + diffuse) + specular); 
}

vec4 phongLightModel(mat4 M){
	Material m = !lightModel.twoSided ?  material[0] : gl_FrontFacing ? material[0] : material[1];
	vec4 color = m.emission + lightModel.globalAmbience;
	// getAmbience(m, vertex_in.texCoord);

	//vec2 uv = get_uv(vertex_in.eyes);
	vec2 uv = vertex_in.texCoord;
	for (int i = 0; i < numLights; i++)
		color += apply(light[i], vertex_in.spotDirection[i], vertex_in.lightDirection[i], m, uv);
	return color;
}

vec4 getAmbience(Material m, vec2 uv){
    vec4 color = m.ambientMap ? m.ambient * texture(ambientMap, uv) : m.ambient;
	return lightModel.colorMaterial ? vertex_in.color : color;
}

vec4 getDiffuse(Material m, vec2 uv){
    vec3 color = m.diffuseMap ? m.diffuse.xyz * pow(texture(diffuseMap, uv).xyz, vec3(2.2)) : m.diffuse.xyz;
	return lightModel.colorMaterial ? vertex_in.color : vec4(color, 1);
}

vec4 getSpecular(Material m, vec2 uv) {
	vec4 color = m.specularMap ? m.specular * texture(specularMap, uv) : m.specular;
	return lightModel.colorMaterial ? vertex_in.color : color;
}

vec2 get_uv(vec3 viewDir) {
	if (!lightModel.useObjectSpace && !displace) {
		return vertex_in.texCoord;
	}
	else {
		return parallaxMapping0(viewDir);
	}
}

vec2 parallaxMapping(vec3 viewDir) {
	float height = 1 - texture(displacementMap, vertex_in.texCoord).r;	// TODO use uniform flag
	vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	return vertex_in.texCoord - p;
}

vec2 parallaxMapping0(vec3 viewDir) {
	const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;
	vec2 p = viewDir.xy /viewDir.z * height_scale;
	vec2 deltaTexCoords = p / numLayers;

	vec2 currentTexCoords = vertex_in.texCoord;
	float currentDepthMapValue = 1 - texture(displacementMap, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = 1 - texture(displacementMap, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1 - texture(displacementMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;
}