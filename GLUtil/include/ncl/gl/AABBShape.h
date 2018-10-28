#pragma once

#include <memory>
#include "Shape.h"
#include "../geom/AABB.h"

namespace ncl {
	namespace gl {
		class AABBShape : public Shape {
		public:
			AABBShape(Shape& s, const glm::vec4 color = randomColor()) :AABBShape(extractFrom(s), color) {}

			AABBShape(geom::bvol::AABB aabb, const glm::vec4 color = randomColor()) :Shape(createMesh(aabb, color)), aabb(aabb) {

			}

			const glm::vec3& min() const {
				return aabb.min();
			}

			const glm::vec3& max() const {
				return aabb.max();
			}

		protected:
			static std::vector<Mesh> createMesh(geom::bvol::AABB aabb, const glm::vec4 color = randomColor()) {
				using namespace glm;
				using namespace std;
				Mesh mesh;
				mesh.positions.push_back(vec3(aabb.min().x, aabb.min().y, aabb.max().z));
				mesh.positions.push_back(vec3(aabb.min().x, aabb.min().y, aabb.min().z));
				mesh.positions.push_back(vec3(aabb.min().x, aabb.max().y, aabb.min().z));
				mesh.positions.push_back(vec3(aabb.min().x, aabb.max().y, aabb.max().z));
				mesh.positions.push_back(vec3(aabb.max().x, aabb.min().y, aabb.min().z));
				mesh.positions.push_back(vec3(aabb.max().x, aabb.max().y, aabb.min().z));
				mesh.positions.push_back(vec3(aabb.max().x, aabb.min().y, aabb.max().z));
				mesh.positions.push_back(vec3(aabb.max().x, aabb.max().y, aabb.max().z));


				GLuint indices[] = {
					3, 2, 2, 5, 5,
					7, 7, 3, 3, 0,
					0, 6, 6, 4, 4,
					1, 1, 0, 0, 3,
					5, 4, 2, 1, 7,
					6
				};

				mesh.indices = vector<GLuint>(begin(indices), end(indices));
				mesh.colors = vector<vec4>(8, color);
				mesh.primitiveType = GL_LINES;
				return vector<Mesh>(1, mesh);
			}

			static geom::bvol::AABB extractFrom(Shape& s) {

				return s.mapTo<geom::bvol::AABB, glm::vec3>(0, Position, [&](glm::vec3* v) {
					return geom::bvol::AABB(v, s.numVertices(0));
				});
			
			}

		public:
			geom::bvol::AABB aabb;
		};
	}
}