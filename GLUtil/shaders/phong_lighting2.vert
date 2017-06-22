
#version 450 core
#pragma debug(on)
#pragma optimize(off)

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
};

struct Material{
	vec4 emission;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalMatrix;

uniform LightSource light0;
uniform Material material;
uniform vec4 globalAmbience;
uniform bool localViewer;
uniform bool eyesAtCamera;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

vec3 lightDirection(vec4 pos, LightSource light);

float daf(float dist, LightSource light);

float saf(LightSource light, vec3 lightDirection);


void main(){
	Material m = material;
	vec3 n = normalize(normalMatrix * normal);
	vec4 pos = (MV * vec4(position, 1));
//	vec3 l = lightDirection(pos, light0);
	vec3 l = normalize((MV*light0.position).xyz - pos.xyz);
	float f = m.shininess;
	
	vec4 gobAmbient = m.emission + globalAmbience * m.ambient;
	
	float _daf = daf(length(l), light0);
	float _saf = saf(light0, l);
	
	vec4 ambient = light0.ambient * m.ambient;
	
	vec4 diffuse = max(dot(l, n), 0) * light0.diffuse * m.diffuse;
	
	vec3 e = normalize(-pos.xyz);
	vec3 s = normalize(l + e);	// half way vector between light direction and eyes
	vec4 specular = pow(max(dot(s, n), 0), f) * light0.specular * m.specular;


	vertex.color = gobAmbient + _daf * _saf * ((ambient + diffuse) + specular); 

	gl_Position = MVP * vec4(position, 1);
}


float daf(float dist, LightSource light){
	return 1.0 / (light.kc + light.ki * dist + light.kq * dist * dist);
}

float saf(LightSource light, vec3 lightDirection){
	vec3 l = normalize(lightDirection);
	vec3 d = normalize(light.spotDirection.xyz);
	float h = light.spotExponent;
	
	if(light.spotAngle >= 180) 	return 1.0;
	
	float _LdotD = dot(-l, d);
	float cos_spotAngle = cos(radians(light.spotAngle));
	
	if(_LdotD < cos_spotAngle) return 0.0;
	
	return pow(_LdotD, h); 
}