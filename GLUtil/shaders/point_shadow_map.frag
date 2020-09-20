#version 450 core 

uniform vec3 lightPos;
uniform float nearPlane;
uniform float farPlane;

in ncl_PerVertexPos{
	smooth vec3 local;
	smooth vec3 world;
	smooth vec3 view;
} pos;

void onFragment(vec3 localPos, vec3 worldPos, vec3 viewPos);

void main(){
	float lightDistance = length(pos.world.xyz - lightPos);

	lightDistance = lightDistance /farPlane;	// map to [0:1]

	gl_FragDepth = lightDistance;

	onFragment(pos.local, pos.world, pos.view);
}