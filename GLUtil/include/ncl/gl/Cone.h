#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class Cone : public Shape {
		public:
			Cone(GLfloat r, float l, GLuint p, GLuint q, const glm::vec4& color = glm::vec4(1)) :
				Shape(createMesh(r, l, p, q, color), false) {
			}

		protected:
			std::vector<Mesh> createMesh(float r, float l, int p, int q, const glm::vec4& color) {
				Mesh mesh;

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
				return r * float(j) / q * cos(2 * float(i) / p * PI);
			}

			float g(int i, int j, int q, int p, float r) {
				return r * float(j) / q * sin(2 * float(i) / p * PI);

			}

			float h(int i, int j, int q, int p, float h) {
				return h *  float(j) / q;
			}

			float fn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q;

				return float(j) / q * cos(2 * float(i) / p * PI);
			}

			float gn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q;

				return float(j) / q * sin(2 * float(i) / p * PI);

			}

			float hn(int i, int j, int q, int p, float h) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q;

				return 0;
			}

			glm::vec3 calculateNormal(int i, int j, int q, int p) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q;
				glm::vec3 du = { -v * sin(u), v * cos(u), 0 };
				glm::vec3 dv = { cos(u), sin(u), 1 };

				return glm::cross(du, dv);
			}

		private:
			static const float PI;
		};

		const float Cone::PI = 3.14159265358979324f;
	}
}