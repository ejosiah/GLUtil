#pragma once

#include "PatchShape.h"
#include <iterator>
#include <glm/glm.hpp>
#include <iostream>

namespace ncl {
	namespace gl {
		class Cube : public Shape {
		public:
			Cube(float size = 1.0f, float grids = 10, const glm::vec4& color = randomColor())
				:Shape(createMesh(size/2, color), true) {}


			std::vector<Mesh> createMesh(float halfSize, const glm::vec4& color) {
				using namespace glm;
				const int NO_VERTICES = 24;

				vec3 positions[NO_VERTICES] = {
					// Front
					vec3(-halfSize, -halfSize, halfSize),
					vec3(halfSize, -halfSize, halfSize),
					vec3(halfSize,  halfSize, halfSize),
					vec3(-halfSize,  halfSize, halfSize),
					// Right
					vec3(halfSize, -halfSize, halfSize),
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
					vec3(halfSize, -halfSize, halfSize),
					// Top
					vec3(-halfSize,  halfSize, halfSize),
					vec3(halfSize,  halfSize, halfSize),
					vec3(halfSize,  halfSize, -halfSize),
					vec3(-halfSize,  halfSize, -halfSize)
				};

				vec3 normals[NO_VERTICES] = {
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

				vec2 texCoords[NO_VERTICES] = {
					// Front
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					// Right
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					// Back
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					// Left
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					// Bottom
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					// Top
					vec2(0.0f, 0.0f),
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f)
				};


				GLuint indices[] = {
					0,1,2,0,2,3,
					4,5,6,4,6,7,
					8,9,10,8,10,11,
					12,13,14,12,14,15,
					16,17,18,16,18,19,
					20,21,22,20,22,23
				};

				Mesh mesh;

				mesh.positions = std::vector<vec3>(std::begin(positions), std::end(positions));
				mesh.normals = std::vector<vec3>(std::begin(normals), std::end(normals));
				mesh.material.diffuse = color;
				mesh.material.ambient = color;
				mesh.uvs[0] = std::vector<vec2>(std::begin(texCoords), std::end(texCoords));
				mesh.indices = std::vector<GLuint>(std::begin(indices), std::end(indices));
				mesh.primitiveType = GL_TRIANGLES;

				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}