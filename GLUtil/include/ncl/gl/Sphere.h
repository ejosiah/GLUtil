#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "Shape.h"
#include "WithTrianglation.h"
#include "WithTriangleAdjacency.h"

namespace ncl {
	namespace gl {
		class Sphere : public Shape {
		public:
			Sphere() = default;

			Sphere(GLfloat r, GLuint p = 50, GLuint q = 50, const glm::vec4& color = randomColor(), unsigned instances = 1, std::vector<glm::mat4> xforms = {}) :
				Shape(createMesh(r, p, q, color, xforms), true, instances){
			}

			Sphere(const Sphere&) = delete;

			Sphere(Sphere&& source) noexcept : Shape(dynamic_cast<Shape&&>(source)) {

			}

			Sphere& operator=(const Sphere&) = delete;

			Sphere& operator=(Sphere&& source) noexcept {
				Shape::transfer(dynamic_cast<Shape&>(source), dynamic_cast<Shape&>(*this));
				return *this;
			}


		protected:
			std::vector<Mesh> createMesh(float r, int p, int q,const glm::vec4& color, std::vector<glm::mat4> xforms) {
				Mesh mesh;

				for (int j = 0; j <= q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.positions.push_back(glm::vec3(f(i, j, q, p, r), g(i, j, q, p, r), h(i, j, q, p, r)));
						mesh.normals.push_back(glm::vec3(fn(i, j, q, p, r), gn(i, j, q, p, r), hn(i, j, q, p, r)));
						mesh.uvs[0].push_back(glm::vec2(float(p - i) / p, float(q - j) / q));
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
				mesh.xforms = xforms;
				return std::vector<Mesh>(1, mesh);
			} 


			float f(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI;

				return r * cos(u) * sin(v);
			}

			float g(int i, int j, int q, int p, float r) {
				float v = float(j) / q * PI;

				return r * cos(v);
			}

			float h(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI;
				return r * sin(u) * sin(v);
			}

			float fn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI;

				return cos(2 * float(i) / p * PI) * sin(float(j) / q * PI);
			}

			float gn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI;

				return cos(float(j) / q * PI);
			}

			float hn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI;

				return  sin(2 * float(i) / p * PI) * sin(float(j) / q * PI);

			}

		private:
			static const float PI;
		};
		
		const float Sphere::PI = 3.1415926535897932384626433832795;
	}
}