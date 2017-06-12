#pragma once
#include "Shape.h"
#include <iterator>

namespace ncl {
	namespace gl {
		class Bounds : public Shape {
		private:
			Mesurements m;
			glm::vec3 center;
		public:
			Bounds(Mesurements m) :m(m), Shape(getMeshes()) {
				glm::vec3 xCenter = (m.maxX - m.minX) * 0.5f + m.minX;
				glm::vec3 yCenter = (m.maxY - m.minY) * 0.5f + m.minY;
				glm::vec3 zCenter = (m.maxZ - m.minZ) * 0.5f + m.minZ;
				center = xCenter + yCenter + zCenter;
			}

			std::vector<Mesh> getMeshes() {
				using namespace glm;
				using namespace std;
				Mesh mesh;
				mesh.positions.push_back(m.minX + m.minY + m.maxZ);
				mesh.positions.push_back(m.minX + m.minY + m.minZ);
				mesh.positions.push_back(m.minX + m.maxY + m.minZ);
				mesh.positions.push_back(m.minX + m.maxY + m.maxZ);
				mesh.positions.push_back(m.maxX + m.minY + m.minZ);
				mesh.positions.push_back(m.maxX + m.maxY + m.minZ);
				mesh.positions.push_back(m.maxX + m.minY + m.maxZ);
				mesh.positions.push_back(m.maxX + m.maxY + m.maxZ);

				/*
				GLuint indices[] = {
					0, 1, 2,
					0, 2, 3,
					1, 5, 2,
					1, 4, 5,
					4, 7, 5,
					4, 6, 7,
					6, 3, 7,
					6, 0, 3,
					2, 5, 7,
					2, 7, 3,
					0, 4, 1,
					0, 6, 1
				};*/

				GLuint indices[] = {
					3, 2, 2, 5, 5,
					7, 7, 3, 3, 0, 
					0, 6, 6, 4, 4, 
					1, 1, 0, 0, 3,
					5, 4, 2, 1, 7,
					6
				};

				mesh.indices = vector<GLuint>(begin(indices), end(indices));
				mesh.colors = vector<vec4>(8, vec4(1));
				mesh.primitiveType = GL_LINES;
				return vector<Mesh>(1, mesh);
			}
		};
	}
}