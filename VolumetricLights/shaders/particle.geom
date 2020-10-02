#version 450 core
#pragma compile(off)

#define NCL_PI 3.1415926535897932384626433832795
#define MAX_VERTICES_PER_INVOCATION 222
#define MAX_VERTICES 126

layout(points, invocations = 2) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

uniform mat4 MVP;

vec3 vertices[121];
vec3 normals[121];
vec2 uvs[121];
int indices[MAX_VERTICES_PER_INVOCATION];

const int Rows = 10;
const int Cols = 10;

uniform float r0 = 1;
uniform float r1 = 5;
uniform float h = 0.2;

subroutine void ShapeFunc(int row, int col, out vec3 vertex, out vec3 normal);

subroutine uniform ShapeFunc shape;

subroutine (ShapeFunc)
void Sphere(int row, int col, out vec3 vertex, out vec3 normal){
	float u = 2 * float(col) / Cols * NCL_PI;
	float v = float(row) / Rows * NCL_PI;
	float r = r0;

	normal.x = cos(u) * sin(v);
	normal.y = cos(v);
	normal.z = sin(u) * sin(v);

	vec3 center = gl_in[0].gl_Position.xyz;

	vertex = center + r * normal;
}

subroutine (ShapeFunc)
void Cylinder(int row, int col, out vec3 vertex, out vec3 normal){
	float theta = (2 * float(col) / Cols - 1) * NCL_PI;
	
	normal.x = cos(theta);
	normal.y = 0;
	normal.z = sin(theta);

	vec3 center = gl_in[0].gl_Position.xyz;
	vertex.x = center.x + r0 * normal.x;
	vertex.y = center.y + h * (-float(row)/Rows);
	vertex.z = center.z + r0 * normal.z;

}

subroutine (ShapeFunc)
void Torus(int row, int col, out vec3 vertex, out vec3 normal){
	float theta =  (2 * (float(col) / Cols) - 1) * NCL_PI;
	float phi =  (2 * (float(row) / Rows) - 1) * NCL_PI;

	vertex.x = (r1 + r0 * cos( phi)) * cos(theta);
	vertex.y = r0 * sin( phi);
	vertex.z = (r1 + r0 * cos (phi)) * sin(theta);

//	vec3 center = gl_in[0].gl_Position.xyz;
//	vertex = center + r1 * normal;
}

void main(){
	
	for(int row = 0; row <= Rows; row++){
		for(int col = 0; col <= Cols; col++){
			vec3 vertex, normal;

			shape(row, col, vertex, normal);
			
			int i = row * (Cols+1) + col;
			vertices[i] = vertex;
			normals[i] = normal;
			uvs[i] = vec2((col - 1)/Cols, (row - 1)/Rows);
		}
	}

	for(int row = 0; row < Rows; row++){
		for(int col = 0; col <= Cols; col++){
			int i0 = row * (Cols+1) * 2 + (col * 2);
			int i1 = row * (Cols+1) * 2 + (col * 2) + 1;

			indices[i0] = (row + 1) * (Rows + 1) + col;
			indices[i1] = row * (Rows + 1) + col;
		}
	}

	int offset = gl_InvocationID * MAX_VERTICES;
	int count = offset;
	int limit = offset  + MAX_VERTICES;
	limit += (MAX_VERTICES_PER_INVOCATION - limit) * gl_InvocationID;

	for(int i = offset; i < limit; i++ ){
		int index = indices[i];
		vec3 pos = vertices[index];
		gl_Position = MVP * vec4(pos, 1);
		EmitVertex();
	}

	EndPrimitive();

}