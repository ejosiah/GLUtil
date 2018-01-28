#pragma once

#include "Shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace ncl {
	namespace gl {
		class Cylinder : public Shape {
		public:
			Cylinder(GLfloat r = 0.5f, float l = 1.0f, GLuint p = 50, GLuint q = 50, const glm::vec4& color = randomColor(), unsigned instances = 1, glm::mat4& transform = glm::mat4(1)) :
				Shape(createMesh(r, l, p, q, color, transform), false, instances) {
			}

		protected:
			std::vector<Mesh> createMesh(float r, float l, int p, int q, const glm::vec4& color, glm::mat4& transform) {
				Mesh mesh;
				mesh.material.diffuse = color;
				mesh.material.ambient = color;
				for (int j = 0; j <= q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.positions.push_back((transform * glm::vec4(f(i, j, q, p, r), g(i, j, q, p, r), h(i, j, q, p, l), 1.0)).xyz);
						mesh.normals.push_back(glm::mat3(transform) * glm::vec3(fn(i, j, q, p, r), gn(i, j, q, p, r), hn(i, j, q, p, l)));
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

				Mesh base;
				for (int i = 0; i <= p; i++) {
					base.positions.push_back((transform * glm::vec4{ r * cos((-1 + 2 * (float)i / p) * PI), r * sin((-1 + 2 * (float)i / p) * PI), 0, 1.0 }).xyz);
					base.normals.push_back(glm::mat3(transform) * glm::vec3{ 0.0, 0.0, 1.0 });
					base.uvs[0].push_back({ 0.5 + 0.5*cos((-1 + 2 * (float)i / p) * PI), 0.5 + 0.5*sin((-1 + 2 * (float)i / p) * PI) });
					base.colors.push_back(color);
				}
				base.primitiveType = GL_TRIANGLE_FAN;
				base.material.diffuse = color;
				base.material.ambient = color;

				Mesh top;
				for (int i = 0; i <= p; i++) {
					top.positions.push_back((transform * glm::vec4{ r * cos((-1 + 2 * (float)i / p) * PI), r * sin((-1 + 2 * (float)i / p) * PI), -l, 1.0 }).xyz);
					top.normals.push_back(glm::mat3(transform) * glm::vec3{ 0.0, 0.0, -1.0 });
					top.uvs[0].push_back({ 0.5 + 0.5*cos((-1 + 2 * (float)i / p) * PI), 0.5 + 0.5*sin((-1 + 2 * (float)i / p) * PI) });
					top.colors.push_back(color);
				}
				top.primitiveType = GL_TRIANGLE_FAN;
				top.material.diffuse = color;
				top.material.ambient = color;

				std::vector<Mesh> meshes;
				meshes.push_back(mesh);
				meshes.push_back(base);
				meshes.push_back(top);

				return meshes;
			}


			float f(int i, int j, int q, int p, float r) {
				return r *  (cos((-1 + 2 * (float)i / p) * PI));
			}

			float g(int i, int j, int q, int p, float r) {
				return r * (sin((-1 + 2 * (float)i / p) * PI));
			}

			float h(int i, int j, int q, int p, float l) {
				return l * (-1 + (float)j / q);
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