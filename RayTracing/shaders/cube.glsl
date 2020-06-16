
const float halfSize = 1;

vec3 vertices[24] = {
	vec3(-halfSize, -halfSize, halfSize),
	vec3(halfSize, -halfSize, halfSize),//
	vec3(halfSize,  halfSize, halfSize),
	vec3(-halfSize,  halfSize, halfSize),
	// Right
	vec3(halfSize, -halfSize, halfSize),//
	vec3(halfSize, -halfSize, -halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(halfSize,  halfSize, halfSize),
	// Back
	vec3(-halfSize, -halfSize, -halfSize),
	vec3(-halfSize,  halfSize, -halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(halfSize, -halfSize, -halfSize),
	// Left
	vec3(-halfSize, -halfSize, halfSize),
	vec3(-halfSize,  halfSize, halfSize),
	vec3(-halfSize,  halfSize, -halfSize),
	vec3(-halfSize, -halfSize, -halfSize),
	// Bottom
	vec3(-halfSize, -halfSize, halfSize),
	vec3(-halfSize, -halfSize, -halfSize),
	vec3(halfSize, -halfSize, -halfSize),
	vec3(halfSize, -halfSize, halfSize),//
	// Top
	vec3(-halfSize,  halfSize, halfSize),
	vec3(halfSize,  halfSize, halfSize),
	vec3(halfSize,  halfSize, -halfSize),
	vec3(-halfSize,  halfSize, -halfSize)
};

vec3 normals[24] = {
	// Front
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f),
	// Right
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 0.0f),
	// Back
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	// Left
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	// Bottom
	vec3(0.0f, -1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f),
	// Top
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f)
};

int indices[36] = {
	0,1,2,0,2,3,
	4,5,6,4,6,7,
	8,9,10,8,10,11,
	12,13,14,12,14,15,
	16,17,18,16,18,19,
	20,21,22,20,22,23
};