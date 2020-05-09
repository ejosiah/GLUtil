#version 450 core 

uniform vec3 lightPos;
uniform float nearPlane;
uniform float farPlane;

in vec4 fragPos;

void main(){
	float lightDistance = length(fragPos.xyz - lightPos);

	lightDistance = lightDistance /farPlane;	// map to [0:1]

	gl_FragDepth = lightDistance;
}