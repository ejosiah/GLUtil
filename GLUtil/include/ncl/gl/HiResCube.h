#pragma once

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Shape.h"


namespace ncl {
	namespace gl {
		class HiResCube : public Shape {
		public:
			HiResCube(int r, int c, float size = 1.0f, glm::vec4& color = glm::vec4{ 1, 1, 1, 1 })
				:Shape(createMesh(r, c, size / 2, color)) {
			}


			std::vector<Mesh> createMesh(int r, int c, float halfSize, const glm::vec4& color) {
				using namespace glm;
				using namespace std;

				vector<vec3> positions;
				vector<vec3> normals;
				vector<vec2> uvs;
				vector<unsigned int> indices(r*c * 2 * 3);
				
				for (int j = 0; j <= r; j++) {
					for (int i = 0; i <= c; i++) {
						glm::vec3 pos{
							(float(i) / (c - 2) * 2 - 1) * halfSize
							, 0
							, (float(j) / (r - 1) * 2 - 1) * halfSize
						};
						glm::vec3 normal = glm::normalize(pos + glm::vec3(0, 1, 0));
						glm::vec2 uv;

						uv = { float(i) / c, float(j) / r };

						positions.push_back(pos);
						normals.push_back(normal);
						uvs.push_back(uv);
					}
				}
				

				GLuint* id = &indices[0];
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

				unsigned int size = positions.size();
				Mesh mesh;
				mesh.material.diffuse = color;
				

				mat4 matrix = translate(mat4(1), vec3(0, halfSize * 2, 0));

				// top face
				
				mesh.positions = vector<vec3>(size * 6);
				mesh.normals = vector<vec3>(size * 6);
				mesh.uvs[0] = vector<vec2>(size * 6);
				mesh.indices = std::vector<GLuint>(r*c * 2 * 3 * 6);


				matrix = mat4(1);
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				matrix = mat4(1);
				matrix = glm::rotate(matrix, -radians(90.f), vec3(0, 0, 1));
				matrix = glm::translate(matrix, vec3(-halfSize * 2, 0, 0));
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				matrix = mat4(1);
				matrix = glm::translate(matrix, vec3(halfSize * 2, 0, 0));
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				matrix = mat4(1);
				matrix = glm::translate(matrix, vec3((halfSize * 2) * 2, 0, 0));
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				matrix = mat4(1);
				matrix = glm::rotate(matrix, radians(-90.f), vec3(1, 0, 0));
				matrix = glm::translate(matrix, vec3(0, 0, halfSize * 2));
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				matrix = mat4(1);
				matrix = glm::rotate(matrix, radians(90.f), vec3(1, 0, 0));
				matrix = glm::translate(matrix, vec3(0, 0, -halfSize * 2));
				for (auto i : indices) {
					vec3 pos = positions[i];
					vec3 norm = normals[i];
					pos = vec3(matrix * vec4(pos, 1));
					norm = inverseTranspose(mat3(matrix)) * norm;
					mesh.positions.push_back(pos);
					mesh.normals.push_back(norm);
					mesh.uvs[0].push_back(uvs[i]);
					mesh.indices.push_back(mesh.positions.size() - 1);
				}

				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}