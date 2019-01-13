#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class CrossHair : public Shape {
		public:
			CrossHair(float size = 1.0f, glm::vec4 color = BLACK)
				:Shape(createMesh(size / 2, color)) {}


			std::vector<Mesh> createMesh(float halfSize, glm::vec4 color) {
				using namespace glm;
				const int NO_VERTICES = 8;

				vec3 positions[NO_VERTICES] = {
					{ -halfSize, 0, 0 },
					{ halfSize, 0, 0 },
					{0, -halfSize, 0},
					{0, halfSize, 0},
					{0, 0, -halfSize},
					{0, 0, halfSize}
				};



				Mesh mesh;

				mesh.positions = std::vector<vec3>(std::begin(positions), std::end(positions));
				mesh.colors = std::vector<vec4>(NO_VERTICES, BLACK);
				mesh.primitiveType = GL_LINES;

				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}