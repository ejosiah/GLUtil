#pragma once

#include "Shape.h"

namespace ncl {
	namespace gl {
		class Plane : public Shape {
		public:
			Plane(int r, int c, float l, float w, bool mapUVtoSize = true, glm::vec4& color = randomColor())
			:Shape(createMesh(w, l, r, c, mapUVtoSize, color)){

			}

			 std::vector<Mesh> createMesh(float w, float l, float r, float c, bool mapUVtoSize, glm::vec4& color) {
				float halfLength = l / 2;
				float halfWidth = w / 2;
				Mesh mesh;
				for (int j = 0; j <= r; j++) {
					for (int i = 0; i <= c; i++) {
						glm::vec3 pos{
							(float(i) / (c - 2) * 2 - 1) * halfWidth
							, 0
							, (float(j) / (r - 1) * 2 - 1) * halfLength
						};
						glm::vec3 normal = glm::normalize(pos + glm::vec3(0, 1, 0));
						glm::vec2 uv;

						if (mapUVtoSize) {
							uv = glm::vec2((float(i) / c) * w, (float(j) / r) * l);
							mesh.uvs[1].push_back({ float(i) / c, float(j) / r });
						}
						else {
							uv = { float(i) / c, float(j) / r };
						}


						mesh.positions.push_back(pos);
						mesh.normals.push_back(normal);
						mesh.uvs[0].push_back(uv);
					}
				}
				mesh.material.diffuse = color;
				mesh.indices = std::vector<GLuint>(r*c * 2 * 3);

				GLuint* id = &mesh.indices[0];
				for (int i = 0; i < r; i++) {
					for (int j = 0; j < c; j++) {
						int v0 = i * (c + 1) + j;
						int v1 = v0 + 1;
						int v2 = v0 + (c + 1);
						int v3 = v2 + 1;
						if ((j + 1) % 2) {
							*id++ = v0;
							*id++ = v2;
							*id++ = v1;
							*id++ = v1;
							*id++ = v2;
							*id++ = v3;
						}
						else {
							*id++ = v0;
							*id++ = v2;
							*id++ = v3;
							*id++ = v0;
							*id++ = v3;
							*id++ = v1;
						}
					}
				}
				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}