#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec4 frustumPlanes[6];

smooth in vec2 uv[3];
smooth in vec3 worldPos[3];

out ncl_PerVertex{
	vec3 worldPos;
	vec3 normal;
	vec2 uv;
} ncl_out;


vec3 calcNormal(){
	vec3 v0 = worldPos[0];
	vec3 v1 = worldPos[1];
	vec3 v2 = worldPos[2];

	vec3 A = v1 - v0;
	vec3 B = v2 - v0;
	vec3 N = cross(A, B);
	return normalize(N);
}

bool pointInFrustum(vec3 p){
	for(int i = 0; i < 6; i++){
		vec4 plane = frustumPlanes[i];
		if(dot(plane.xyz, p) + plane.w < 0){
			return false;
		}
	}
	return true;
}

bool triangleInFrustum(){
	return all(bvec3(
		pointInFrustum(worldPos[0]),
		pointInFrustum(worldPos[1]),
		pointInFrustum(worldPos[2])
	));
}

void main(){

	vec3 n = calcNormal();

	for(int i = 0; i < 3; i++){
		gl_Position = gl_in[i].gl_Position;
		ncl_out.uv = uv[i];
		ncl_out.normal = n;
		ncl_out.worldPos = worldPos[i];
		EmitVertex();
	}
	EndPrimitive();
}