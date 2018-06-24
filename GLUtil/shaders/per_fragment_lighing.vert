#version 450 core
#pragma debug(on)
#pragma optimize(off)

const int MAX_LIGHT_SOURCES = 10;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;

uniform struct LightSource{
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
} light[MAX_LIGHT_SOURCES];

struct LightModel {
	bool localViewer;
	bool twoSided;
	bool useObjectSpace;
	bool celShading;
	vec4 globalAmbience;
	bool colorMaterial;
};


out VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
	smooth vec3 eyes;
	smooth vec4 lightDirection[MAX_LIGHT_SOURCES];
} vertex;

noperspective out vec3 edgeDistance;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform LightModel lightModel;
uniform mat3 normalMatrix;
uniform bool useObjectSpace;

mat3 OLM;
mat4 MV;
mat4 MVP;
mat3 NM;

vec4 getLightDirection(vec4 pos, in LightSource light){
	vec4 direction = vec4(0);
	if(light.position.w == 0){	// directional light
		if(light.transform){
			direction = V * light.position;
		}
		direction = light.position;
	}
	else{	// positional light
		vec4 lightPos = (light.position/light.position.w);
		if(light.transform){
			direction = (V*light.position) - pos;
		}else{
			direction = light.position - pos;
		}
	}
	return normalize(vec4( OLM * direction.xyz, 1.0));
}

void main(){
	vec3 n = normalize(normalMatrix * normal);
	vec3 t = normalize(normalMatrix * tangent);
	vec3 b = normalize(normalMatrix * bitangent);

	MV = V * M;
	MVP = P * MV;

	vertex.normal =  n;
	vec4 pos = MV * vec4(position, 1);
	vertex.position = pos.xyz;
	
	OLM = !lightModel.useObjectSpace ? mat3(1) : mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);

	for(int i = 0; i < light.length(); i++){
		vertex.lightDirection[i] = getLightDirection(pos, light[i]);
	}

	vertex.eyes =  OLM * (lightModel.localViewer ? normalize(-pos.xyz) : vec3(0, 0, 1));

	vertex.texCoord = uv;
	vertex.color = color;
	gl_Position = MVP * vec4(position, 1);
}