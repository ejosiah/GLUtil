#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class Cylinder : public Shape {
		public:
			Cylinder(GLfloat r, float l, GLuint p, GLuint q, const glm::vec4& color = glm::vec4(1)) :
				Shape(createMesh(r, l, p, q, color), false) {
			}

		protected:
			std::vector<Mesh> createMesh(float r, float l, int p, int q, const glm::vec4& color) {
				Mesh mesh;
				mesh.material.diffuse = color;
				mesh.material.ambient = color;
				for (int j = 0; j <= q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.positions.push_back(glm::vec3(f(i, j, q, p, r), g(i, j, q, p, r), h(i, j, q, p, l)));
						mesh.normals.push_back(glm::vec3(fn(i, j, q, p, r), gn(i, j, q, p, r), hn(i, j, q, p, l)));
						mesh.uvs[0].push_back(glm::vec2(float(i) / p, float(j) / q));
						mesh.colors.push_back(color);
					}
				}

				for (int j = 0; j < q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.indices.push_back((j + 1)*(p + 1) + i);
						mesh.indices.push_back(j*(p + 1) + i);
					}
				}
				mesh.primitiveType = GL_TRIANGLE_STRIP;
				return std::vector<Mesh>(1, mesh);
			}


			float f(int i, int j, int q, int p, float r) {
				return r *  (cos((-1 + 2 * (float)i / p) * PI));
			}

			float g(int i, int j, int q, int p, float r) {
				return r * (sin((-1 + 2 * (float)i / p) * PI));
			}

			float h(int i, int j, int q, int p, float l) {
				return l * (-1 + 2 * (float)j / q);
			}

			float fn(int i, int j, int q, int p, float r) {
				return cos((-1 + 2 * (float)i / p) * PI);
			}

			float gn(int i, int j, int q, int p, float r) {
				return sin((-1 + 2 * (float)i / p) * PI);
			}

			float hn(int i, int j, int q, int p, float h) {
				return 0;
			}

		private:
			static const float PI;
		};

		const float Cylinder::PI = 3.14159265358979324f;
	}
}