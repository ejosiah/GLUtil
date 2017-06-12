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

layout(location=0) in vec3 vVertex;		//per-vertex position
layout(location=1) in vec3 vNormal;		//per-vertex normal
 
//uniforms  
uniform mat4 MVP;				//combined modelview projection matrix
uniform mat4 MV;				//modelview matrix
uniform mat3 normalMatrix;					//normal matrix
uniform LightSource light0;
uniform Material material;

//shader outputs to the fragment shader
smooth out vec4 color;    //final diffuse colour to the fragment shader

//shader constant
const vec3 vEyeSpaceCameraPosition = vec3(0,0,0); //eye is at vec3(0,0,0) in eye space

void main()
{ 	

	vec3 light_position = light0.position.xyz;
	vec3 diffuse_color = material.diffuse.xyz;
	vec3 specular_color = material.specular.xyz;
	float shininess = material.shininess;
	mat3 N = normalMatrix;

	//multiply the object space light position with the modelview matrix 
	//to get the eye space light position
	vec4 vEyeSpaceLightPosition = MV*vec4(light_position,1);

	//multiply the object space vertex position with the modelview matrix 
	//to get the eye space vertex position
	vec4 vEyeSpacePosition = MV*vec4(vVertex,1); 

	//multiply the object space normal with the normal matrix 
	//to get the eye space normal
	vec3 vEyeSpaceNormal   = normalize(N*vNormal);

	//get the light vector
	vec3 L = normalize(vEyeSpaceLightPosition.xyz-vEyeSpacePosition.xyz);
	//get the view vector
	vec3 V = normalize(vEyeSpaceCameraPosition.xyz-vEyeSpacePosition.xyz);
	//get the half way vector between light and view vectors
	vec3 H = normalize(L+V);

	//calculate the diffuse and specular components
	float diffuse = max(0, dot(vEyeSpaceNormal, L));
	float specular = max(0, pow(dot(vEyeSpaceNormal, H), shininess));

	//calculate the final colour by adding the diffuse and specular components
	color = diffuse*vec4(diffuse_color,1) + specular*vec4(specular_color, 1);

	//multiply the combiend modelview projection matrix with the object space vertex
	//position to get the clip space position
    gl_Position = MVP*vec4(vVertex,1); 
}
 