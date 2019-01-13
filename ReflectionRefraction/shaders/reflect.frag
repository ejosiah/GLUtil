#version 450 core
#pragma debug(on)
#pragma optimize(off)

struct AABB;

bool triangleRayIntersect(vec3 a, vec3 b, vec3 c, vec3 p, vec3 q, bool lineTest, out float u, out float v, out float w, out float t);

#pragma include("triangle_ray_intersection.glsl")
#pragma include("aabb.glsl")

layout(binding=0) uniform samplerBuffer triangles;
layout(binding=7) uniform samplerCube skybox;


struct BVHNode{
  AABB box;
  int id;
  int offset;
  int size;
  int isLeaf;
  int child[2];
};

layout(std430, binding=0) buffer BVH_SSBO{
    BVHNode bvh[];
};


layout(std430, binding=1) buffer BVH_TRI_INDEX_SSBO{
    int index[];
};

in VERTEX {
	smooth vec3 position;
	smooth vec3 normal;
	smooth vec2 texCoord;
	smooth vec4 color;
} vertex;

out vec4 fragColor;

uniform bool reflectMode = false;
uniform bool doubleRefract = true;
uniform int numTriangles;
uniform mat4 M;
uniform mat4 V;

uniform vec3 tint = vec3(0, 1, 0);

void main(){
	vec3 I = vertex.position;
	vec3 N = normalize(vertex.normal);
	vec3 uv;
	if(reflectMode){
		uv = reflect(I, N);
	}else{
		float air = 1.0;
		float glass = 1.57;
		float diamond = 2.42;
		float eoa = doubleRefract ?  air/diamond : air/glass;
		uv = refract(I, N, eoa);
	}
	fragColor = vec4(texture(skybox, uv).xyz, 1.0);
}