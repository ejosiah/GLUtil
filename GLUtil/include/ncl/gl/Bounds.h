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

				center = m.max - m.min * 0.5f + m.min;
			}

			std::vector<Mesh> getMeshes() {
				using namespace glm;
				using namespace std;
				Mesh mesh;
				mesh.positions.push_back(vec3(m.min.x, m.min.y, m.max.z));
				mesh.positions.push_back(vec3(m.min.x , m.min.y , m.min.z));
				mesh.positions.push_back(vec3(m.min.x , m.max.y , m.min.z));
				mesh.positions.push_back(vec3(m.min.x , m.max.y , m.max.z));
				mesh.positions.push_back(vec3(m.max.x , m.min.y , m.min.z));
				mesh.positions.push_back(vec3(m.max.x , m.max.y , m.min.z));
				mesh.positions.push_back(vec3(m.max.x , m.min.y , m.max.z));
				mesh.positions.push_back(vec3(m.max.x , m.max.y , m.max.z));

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

			glm::vec3 min() { return m.min;  }
			glm::vec3 max() { return m.max;  }
		};
	}
}