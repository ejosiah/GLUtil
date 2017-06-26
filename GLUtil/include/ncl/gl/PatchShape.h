#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class PatchShape : public Shape {
		public:
			PatchShape(std::vector<Mesh> meshes, int numVertices, float* outer, float* inner, bool cullface = true) :
				Shape(meshes, cullface), nVertics(numVertices), outer(outer), inner(inner) {

			}

			virtual ~PatchShape() {
				delete[] outer;
				delete[] inner;
			}

			virtual void draw(Shader& shader) override {
				glPatchParameteri(GL_PATCH_VERTICES, nVertics);
				glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outer);
				glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, inner);
				Shape::draw(shader);
			}
		private:
			int nVertics;
			float* outer;
			float* inner;
		};
	}
}