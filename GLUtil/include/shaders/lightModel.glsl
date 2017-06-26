#ifdef LIGHT_MODEL_GLSL
#define CONSTANTS_GLSL

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

struct LightModel {
	bool localViewer;
	bool twoSided;
	bool useObjectSpace;
	bool celShading;
	vec4 globalAmbience;
	bool colorMaterial;
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

uniform LightSource light[MAX_LIGHT_SOURCES];
uniform LightModel lightModel;
uniform Material material[2];

#endif