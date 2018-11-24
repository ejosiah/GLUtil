#pragma include("constants.glsl")
#pragma include("lightModel.glsl")
#pragma include("vertex_in.glsl")

layout(binding = 10) uniform sampler2D ambientMap;
layout(binding = 11) uniform sampler2D diffuseMap;
layout(binding = 12) uniform sampler2D specularMap;
layout(binding = 13) uniform sampler2D normalMap;
layout(binding = 14) uniform sampler2D displacementMap;
layout(binding = 15) uniform sampler2D reflectionMap;
layout(binding = 16) uniform sampler2D ambiantOcclusionMap;

vec4 getAmbience(Material m);
vec4 getDiffuse(Material m);
vec4 getSpecular(Material m);
vec4 diffuseContrib(vec3 L, vec3 N, LightSource light, Material m);

float daf(float dist, LightSource light){
	if (light.w == 0) return 1;
	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);
}

float saf(LightSource light, vec3 lightDirection){
	if (light.w == 0) return 1;
	vec3 l = normalize(lightDirection);
	vec3 d =   normalize(light.spotDirection.xyz);
	float h = light.spotExponent;
	
	if(light.spotAngle >= 180) 	return 1.0;
	
	float _LdotD = dot(-l, d);
	float cos_spotAngle = cos(radians(light.spotAngle));
	
	if(_LdotD < cos_spotAngle) return 0.0;
	
	return pow(_LdotD, h); 
}


vec4 apply(LightSource light, vec4 direction, Material m){
	if(!light.on) return vec4(0);
	vec3 n = gl_FrontFacing ? normalize(vertex_in.normal) : normalize(-vertex_in.normal);
	vec3 N = lightModel.useObjectSpace ? (2.0 * texture(normalMap, vertex_in.texCoord) - 1.0).xyz : n;
	vec3 L = normalize(direction.xyz);
	float f = m.shininess;
		
	float _daf = daf(length(L), light);

	float _saf = saf(light, L);
	
	vec4 ambient = light.ambient * m.ambient;
	
	vec4 diffuse =  diffuseContrib(L, N, light, m);
	
	vec3 E = normalize(vertex_in.eyes);
	vec3 S = normalize(L + E);	// half way vector between light direction and eyes
	vec4 specular = pow(max(dot(S, N), 0), f) * light.specular * m.specular;

	return  _daf * _saf * ((ambient + diffuse) + specular); 
}

vec4 phongLightModel(mat4 M){
	Material m = !lightModel.twoSided ?  material[0] : gl_FrontFacing ? material[0] : material[1];
	vec4 color = m.emission + lightModel.globalAmbience * getAmbience(m);

	for(int i = 0; i < numLights; i++ )
		color += apply(light[i], vertex_in.lightDirection[i], m);

	return color;
}

vec4 getAmbience(Material m){
    vec4 color = m.ambientMap ? m.ambient * texture(ambientMap, vertex_in.texCoord) : m.ambient;
	return lightModel.colorMaterial ? vertex_in.color : color;
}

vec4 getDiffuse(Material m){
    vec4 color = m.diffuseMap ? m.diffuse * texture(diffuseMap, vertex_in.texCoord) : m.diffuse;
	return lightModel.colorMaterial ? vertex_in.color : color;
}

vec4 getSpecular(Material m) {
	vec4 color = m.specularMap ? m.specular * texture(specularMap, vertex_in.texCoord) : m.specular;
	return lightModel.colorMaterial ? vertex_in.color : color;
}