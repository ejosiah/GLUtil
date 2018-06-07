#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class PatchShape : public Shape {
		public:
			PatchShape(std::vector<glm::vec3> points, int numVertices, float grids, const glm::vec4& color = randomColor(), unsigned instances = 1, bool cullface = true):
				PatchShape(points, numVertices, { grids, grids, grids, grids }, { grids, grids }, color, instances, cullface){}

			PatchShape(std::vector<glm::vec3> points, int numVertices, std::vector<float> outer, std::vector<float> inner, const glm::vec4& color = randomColor(), unsigned instances = 1, bool cullface = true) :
				Shape(createMesh(points, color), cullface, instances), nVertics(numVertices), outer(outer), inner(inner) {

			}

			std::vector<Mesh> createMesh(const std::vector<glm::vec3>& points, const glm::vec4& color) {
				Mesh m;
				m.positions = points;
				m.primitiveType = GL_PATCHES;
				m.material.diffuse = color;
				m.material.ambient = color;
				return std::vector<Mesh>(1, m);
			}

			virtual ~PatchShape() = default;




			virtual void draw(Shader& shader) override {
				glPatchParameteri(GL_PATCH_VERTICES, nVertics);
				glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, &outer[0]);
				glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, &inner[0]);
				Shape::draw(shader);
			}
		private:
			int nVertics;
			std::vector<float> outer;
			std::vector<float> inner;
		};
	}
}