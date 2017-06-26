#pragma once

#include "PatchShape.h"
#include <iterator>
#include <glm/glm.hpp>
#include <iostream>

namespace ncl {
	namespace gl {
		class Cube : public PatchShape {
		public:
			Cube(float size = 1.0f, float grids = 10, const glm::vec4& color = BLACK)
				:PatchShape(createMesh(size / 2, color), 4
					, new float[4]{ grids, grids, grids, grids }
					, new float[2]{ grids, grids }) {}


			std::vector<Mesh> createMesh(float halfSize, const glm::vec4& color) {
				using namespace glm;
				const int NO_VERTICES = 8;

				vec3 positions[NO_VERTICES] = {
					{ -halfSize, 0, halfSize },
					{ halfSize, 0, halfSize },
					{ halfSize, 2 * halfSize, halfSize },
					{ -halfSize, 2 * halfSize, halfSize },
					{ halfSize, 0, -halfSize },
					{ halfSize, 2 * halfSize, -halfSize },
					{ -halfSize, 0, -halfSize },
					{ -halfSize, 2 * halfSize, -halfSize },
				};


				GLuint indices[] = {
					0, 1, 2, 3,
					1, 4, 5, 2,
					4, 6, 7, 5,
					6, 0, 3, 7,
					3, 2, 5, 7,
					6, 4, 1, 0
				};

				Mesh mesh;

				mesh.positions = std::vector<vec3>(std::begin(positions), std::end(positions));
				mesh.colors = std::vector<vec4>(NO_VERTICES, color);
				mesh.indices = std::vector<GLuint>(std::begin(indices), std::end(indices));
				mesh.primitiveType = GL_PATCHES;

				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}