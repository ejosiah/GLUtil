#version 450 core
#pragma degub(on)
#pragma optimize(off)
#pragma compile(off)

layout(triangles) in;

layout(line_strip, max_vertices = 128) out;

in ncl_PerVertex{
	smooth vec2 uv;
	smooth float val;
	smooth vec3 normal;
	smooth vec3 position;
} ncl_in[3];


uniform int id = 0;
uniform float minVal;
uniform float maxVal;
uniform int numSteps;
uniform mat4 MVP;

bool intersects(vec3 a, vec3 b, vec3 n, float d, out vec3 p);

void main(){

	vec3 n = vec3(0, 1, 0);
	float _step = (maxVal - minVal)/numSteps;
	for(float d = minVal; d <= maxVal; d += _step){
		vec3 p;
		bool hit = false;
		for(int i = 0; i < 3; i++){
			vec3 a = ncl_in[i].position;
			vec3 b = ncl_in[(i+1)%3].position;
			if(intersects(a, b, n, d, p)){
				hit = true;
				gl_ViewportIndex = id;
				gl_Position = MVP * vec4(p.x, 0, p.z, 1);
				EmitVertex();
			}
		}
		if(hit){
			EndPrimitive();
			return;
		}
	}
	EndPrimitive();
}

bool intersects(vec3 a, vec3 b, vec3 n, float d, out vec3 p){
	vec3 ab = b - a;
	float t = (d - dot(n, a)) / dot(n, ab);

	if(t >= 0 && t <= 1){
		p = a + ab * t;
		return true;
	}
	return false;
}