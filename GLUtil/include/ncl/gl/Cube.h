#pragma once

#include "PatchShape.h"
#include <iterator>
#include <glm/glm.hpp>
#include <iostream>

namespace ncl {
	namespace gl {
		class Cube : public Shape {
		public:
			Cube() = default;

			Cube(float size, glm::mat4 model, const glm::vec4& color = randomColor())
				:Cube(size, color, { model }, false) {}

			Cube(float size, const glm::vec4& color = randomColor(), std::vector<glm::mat4> models = { glm::mat4{1} }, bool cullface = true)
				:Shape(createMesh(size / 2, std::vector<glm::vec4>{6, color}, models), cullface, models.size()) {}


			Cube(float size, std::vector<glm::vec4> colors, std::vector<glm::mat4> models = { glm::mat4{1} }, bool cullface = true)
				:Shape(createMesh(size / 2, colors, models), cullface, models.size()) {}

			Cube(const Cube&) = delete;

			Cube(Cube&& source) noexcept : Shape(dynamic_cast<Shape&&>(source)) {

			}

			Cube& operator=(const Cube&) = delete;

			Cube& operator=(Cube&& source) noexcept {
				Shape::transfer(dynamic_cast<Shape&>(source), dynamic_cast<Shape&>(*this));
				return *this;
			}


			std::vector<Mesh> createMesh(float halfSize, const std::vector<glm::vec4> colors, std::vector<glm::mat4> models) {
				using namespace glm;
				const int NO_VERTICES = 24;

				std::string names[]{"front", "right", "back", "left", "bottom", "top"};

				vec3 positions[NO_VERTICES] = {
					// Front
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
				mesh.material.diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
				mesh.uvs[0] = std::vector<vec2>(std::begin(texCoords), std::end(texCoords));
				mesh.indices = std::vector<GLuint>(std::begin(indices), std::end(indices));
				mesh.primitiveType = GL_TRIANGLES;
				mesh.xforms = std::vector<glm::mat4>(models);

				for (int i = 0; i < 6; i++) {
					for (int j = 0; j < 4; j++) {
						mesh.colors.push_back(colors[i]);
					}
				}

				return std::vector<Mesh>(1, mesh);

			}
		};
	}
}