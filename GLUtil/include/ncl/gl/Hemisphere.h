#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "Shape.h"
#include "logger.h"

namespace ncl {
	namespace gl {
		class Hemisphere : public Shape {
		public:
			Hemisphere(GLfloat r, GLuint p, GLuint q, const glm::vec4& color = glm::vec4(1)) :
				Shape(createMesh(r, p, q, color), false){
			}

		protected:
			std::vector<Mesh> createMesh(float r, int p, int q, const glm::vec4& color) {
				using namespace std;
				Mesh mesh;
				int k = 0;
				for (int j = 0; j <= q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.positions.push_back(glm::vec3(f(i, j, q, p, r), g(i, j, q, p, r), h(i, j, q, p, r)));
						mesh.normals.push_back(-glm::vec3(fn(i, j, q, p, r), gn(i, j, q, p, r), hn(i, j, q, p, r)));
						mesh.uvs[0].push_back(glm::vec2(float(p - i) / p, float(q - j) / q));
						mesh.colors.push_back(color);

						
						glm::vec3 v = *(mesh.positions.end() - 1);
						std::string msg =  to_string(k) + " v(" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + ")";
						k++;
					}
				}

				for (int i = 0; i < mesh.positions.size(); i++) {
					glm::vec3 v = mesh.positions[i];
					for (int j = i + 1; j < mesh.positions.size(); j++) {
						glm::vec3 v1 = mesh.positions[j];
						if (closeEnough(v.x, v1.x) && closeEnough(v.y, v1.y) && closeEnough(v.z, v1.z)) {
							mesh.positions[j] = v;
						}
					}
				}

				for (int j = 0; j < q; j++) {
					for (int i = 0; i <= p; i++) {
						mesh.indices.push_back((j + 1)*(p + 1) + i);
						mesh.indices.push_back(j*(p + 1) + i);
					}
				}

				for (int i = 0; i < mesh.indices.size(); i ++) {
					std::string msg =  to_string(mesh.indices[i]);
				}
				mesh.primitiveType = GL_TRIANGLE_STRIP;
				return std::vector<Mesh>(1, mesh);
			} 


			float f(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI/2.0;

				return r * cos(u) * cos(v);
			}

			float g(int i, int j, int q, int p, float r) {
				float v = float(j) / q * PI/2.0;

				return r * sin(v);
			}

			float h(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI/2;
				return r * sin(u) * cos(v);
			}

			float fn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI/2;

				return cos(u) * cos(v);
			}

			float gn(int i, int j, int q, int p, float r) {
				float v = float(j) / q * PI/2;

				return sin(v);
			}

			float hn(int i, int j, int q, int p, float r) {
				float u = 2 * float(i) / p * PI;
				float v = float(j) / q * PI/2;

				return  sin(u) * cos(v);

			}

		private:
			static const float PI;
			Logger logger;
		};
		
		const float Hemisphere::PI = 3.142857143f;
	}
}
