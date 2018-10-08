#pragma once

#include "Shape.h"
#include "..\geom\Plane.h"
#include "..\..\glm\vec_util.h"
#include <glm\glm.hpp>

namespace ncl {
	namespace gl {
		class Plane : public Shape {
		public:
			Plane(const geom::Plane& plane, float scale = 1.0f, const glm::vec4& color = randomColor())
				:Shape(from(plane, scale, color)){
			
			}

			Plane(int r, int c, float l, float w, const glm::vec4& color = randomColor(), bool mapUVtoSize = true)
			:Shape(createMesh(w, l, r, c, mapUVtoSize, color)){

			}

			std::vector<Mesh> from(const geom::Plane& plane, float s, const glm::vec4& color) {
				std::vector<Mesh> meshes = createMesh(1.0f, 1.0f, 10.0f, 10.0f, false, color);
				Mesh& m = meshes.at(0);
				
				glm::vec3 n1 = plane.n;
				glm::vec3 n2 = m.normals[0];
				glm::vec3 axis = glm::normalize(glm::cross(n1, n2));
				if (glm::abs(n1) == glm::abs(n2)) { // TODO this is wrong, its assuming the vector is at (1, 0, 0,), (0, 1, 0), (0, 0, 1)
					axis = glm::vec3(1, 0, 0);
				}

				float angle = -glm::degrees(glm::acos(dot(n1, n2)));

				auto scale = glm::scale(glm::mat4(1), { s, s, s });
				auto translate = glm::translate(glm::mat4(1), plane.d * n1);
				auto rotate = glm::mat4_cast(fromAxisAngle(axis, angle));
				if (n1 == n2) {
					rotate = glm::mat4(1);
				}

				size_t size = m.positions.size();

				for (int i = 0; i < size; i++) {
					auto& p = m.positions[i];
					m.positions[i] =  (translate * rotate * scale *  glm::vec4(p, 1.0f)).xyz;
					m.normals[i] = plane.n;
				}
				return meshes;
			}

			 std::vector<Mesh> createMesh(float w, float l, float r, float c, bool mapUVtoSize, const glm::vec4& color) {
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
						glm::vec3 normal = glm::vec3(0, 1, 0);
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