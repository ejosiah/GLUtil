#version 450 core
#pragma debug(on)
#pragma optimize(off)

const int MAX_LIGHT_SOURCES = 10;
const int MAX_TEXTURES = 8;


layout(binding = 0) uniform sampler2D image0;
layout(binding = 1) uniform sampler2D image1;   // normalMap if using object  space
layout(binding=2) uniform sampler2D image2;
layout(binding=3) uniform sampler2D image3;
layout(binding=4) uniform sampler2D image4;
layout(binding=5) uniform sampler2D image5;
layout(binding=6) uniform sampler2D image6;
layout(binding=7) uniform sampler2D image7;

struct LightSource{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 spotDirection;
	float spotAngle;
	float spotExponent;
	float kc;
	float ki;
	float kq;
	bool transform;
	bool on;
};

struct Material{
	vec4 emission;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	int diffuseMat;
	int specularMat;
	int ambientMat;
	int bumpMap;
};

struct LineInfo{
	float width;
	vec4 color;
};

struct ToonShader{
	int levels;
	float scaleFactor;
};

struct LightModel {
	bool localViewer;
	bool twoSided;
	bool useObjectSpace;
	bool celShading;
	vec4 globalAmbience;
	bool colorMaterial;
};

uniform mat4 MV;
uniform mat4 V;
uniform LightSource light[MAX_LIGHT_SOURCES];
uniform Material material[2];
uniform LineInfo line;
uniform bool wireframe;
uniform LightModel lightModel;
uniform bool activeTextures[MAX_TEXTURES];
uniform int blendTex[MAX_TEXTURES - 1];
uniform int numLights = 1;

ToonShader toonShader;

in VERTEX{
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
} vertex;
noperspective in vec3 edgeDistance;

out vec4 fragColor;

vec4 getAmbience(Material m){
	return lightModel.colorMaterial ? vertex.color : m.ambient;
}

vec4 getDiffuse(Material m){
	return lightModel.colorMaterial ? vertex.color : m.diffuse;
}

float daf(float dist, LightSource light){
	if(light.position.w == 0) return 1;
	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);
}

float saf(LightSource light, vec3 lightDirection){
	if(light.position.w == 0) return 1;
	vec3 l = normalize(lightDirection);
	//vec3 d =   normalize(mat3(V) * light.spotDirection.xyz);
	vec3 d =   normalize(light.spotDirection.xyz);
	float h = light.spotExponent;
	
	if(light.spotAngle >= 180) 	return 1.0;
	
	float _LdotD = dot(-l, d);
	float cos_spotAngle = cos(radians(light.spotAngle));
	
	if(_LdotD < cos_spotAngle) return 0.0;
	
	return pow(_LdotD, h); 
}

float getLineMixColor(){
	float d = min( min(edgeDistance.x, edgeDistance.y), edgeDistance.z);
	return smoothstep(line.width - 1, line.width + 1, d);
}

vec4 apply(LightSource light, vec4 direction, Material m){
	if(!light.on) return vec4(0);
	toonShader.levels = 3;
	toonShader.scaleFactor = 1.0 / toonShader.levels;
	vec3 n = gl_FrontFacing ? normalize(vertex.normal) : normalize(-vertex.normal);
	n = lightModel.useObjectSpace ? (2.0 * texture(image1, vertex.texCoord) - 1.0).xyz : n;
//	n = (2.0 * texture(image1, vertex.texCoord) - 1.0).xyz;
	vec3 l = normalize(direction.xyz);
	float f = m.shininess;
		
	float _daf = daf(length(l), light);

	float _saf = saf(light, l);
	
	vec4 ambient = light.ambient * m.ambient;
	
	vec4 diffuse =  lightModel.celShading ? floor(max(dot(l, n), 0) * toonShader.levels) * toonShader.scaleFactor * light.diffuse *  getDiffuse(m)
							: max(dot(l, n), 0)  * light.diffuse * m.diffuse;
	
	vec3 e = normalize(vertex.eyes);
	vec3 s = normalize(l + e);	// half way vector between light direction and eyes
	vec4 specular = pow(max(dot(s, n), 0), f) * light.specular * m.specular;

	return  _daf * _saf * (ambient + diffuse + specular); 
}

vec4 phongLightModel(){
	Material m = !lightModel.twoSided ?  material[0] : gl_FrontFacing ? material[0] : material[1];
	vec4 color = m.emission + lightModel.globalAmbience * getAmbience(m);

	for(int i = 0; i < numLights; i++ ) 
		color += apply(light[i], vertex.lightDirection[i], m);

	return color;
}

vec4 texColor(){
	vec4 colors[] = {
		texture(image0, vertex.texCoord), texture(image1, vertex.texCoord)
		, texture(image2, vertex.texCoord), texture(image3, vertex.texCoord)
		, texture(image4, vertex.texCoord), texture(image5, vertex.texCoord)
		, texture(image6, vertex.texCoord), texture(image7, vertex.texCoord)
	};	// TODO use texCoord array

	vec4 color = vec4(1);
	int next = 0;
	for(; next < MAX_TEXTURES; next++){
		if(activeTextures[next]){
			color = colors[next];
			break;
		}
	}
//	for(int i = 0; i < blendTex.length(); i++){
//		int blendId = blendTex[i];
//		if(blendId != 0){
//			vec4 blendColor = colors[blendId];
//			color = mix(color, blendColor, blendColor.a);
//		}
//	}
	return color;
}

void main(){
	fragColor = phongLightModel() * texColor();
	//fragColor = wireframe ? mix(line.color, fragColor, getLineMixColor()) : fragColor;
//	fragColor = texture(image0, vertex.texCoord);
}
