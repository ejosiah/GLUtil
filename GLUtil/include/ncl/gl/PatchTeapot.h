#pragma once

#include <glm\glm.hpp>
#include "Shape.h"
#include "teapotdata.h"

namespace ncl {
	namespace gl {
		class PatchTeapot : public Shape {
		public:
			Shader shader;

			PatchTeapot(bool createShader = true, bool normalize = true, const color& color = glm::vec4(1))
				:Shape(createMesh(normalize, color), false) {
				if (createShader) {
					shader.loadFromFile(GL_VERTEX_SHADER, "shaders/teapot.vert");
					shader.loadFromFile(GL_TESS_CONTROL_SHADER, "shaders/teapot.tcs");
					shader.loadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/teapot.tes");
					shader.loadFromFile(GL_GEOMETRY_SHADER, "shaders/wireframe.geom");
					shader.loadFromFile(GL_FRAGMENT_SHADER, "shaders/per_fragment_lighing.frag");
					shader.createAndLinkProgram();
				}
			}

			virtual void draw(Shader& shader) override {
				glPatchParameteri(GL_PATCH_VERTICES, 16);
				Shape::draw(shader);
			}

		protected:
			std::vector<Mesh> createMesh(bool _normalize, const color& color) {

				Mesh mesh;
				mesh.positions = std::vector<glm::vec3>(32 * 16);
				mesh.material.diffuse = color;
				mesh.primitiveType = GL_PATCHES;
				generatePatches((float*)&mesh.positions[0]);

				std::vector<Mesh> meshes = std::vector<Mesh>(1, mesh);

				if(_normalize) normalize(meshes, 1);

				return meshes;
			}

			void generatePatches(float * v) {
				int idx = 0;

				// Build each patch
				// The rim
				buildPatchReflect(0, v, idx, true, true);
				// The body
				buildPatchReflect(1, v, idx, true, true);
				buildPatchReflect(2, v, idx, true, true);
				// The lid
				buildPatchReflect(3, v, idx, true, true);
				buildPatchReflect(4, v, idx, true, true);
				// The bottom
				buildPatchReflect(5, v, idx, true, true);
				// The handle
				buildPatchReflect(6, v, idx, false, true);
				buildPatchReflect(7, v, idx, false, true);
				// The spout
				buildPatchReflect(8, v, idx, false, true);
				buildPatchReflect(9, v, idx, false, true);
			}

			void buildPatchReflect(int patchNum,
				float *v, int &index, bool reflectX, bool reflectY)
			{
				glm::vec3 patch[4][4];
				glm::vec3 patchRevV[4][4];
				getPatch(patchNum, patch, false);
				getPatch(patchNum, patchRevV, true);

				// Patch without modification
				buildPatch(patchRevV, v, index, glm::mat3(1.0f));

				// Patch reflected in x
				if (reflectX) {
					buildPatch(patch, v,
						index, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, 1.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f)));
				}

				// Patch reflected in y
				if (reflectY) {
					buildPatch(patch, v,
						index, glm::mat3(glm::vec3(1.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, -1.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f)));
				}

				// Patch reflected in x and y
				if (reflectX && reflectY) {
					buildPatch(patchRevV, v,
						index, glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, -1.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f)));
				}
			}

			void buildPatch(glm::vec3 patch[][4],
				float *v, int &index, glm::mat3 reflect)
			{
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						glm::vec3 pt = reflect * patch[i][j];

						v[index] = pt.x;
						v[index + 1] = pt.y;
						v[index + 2] = pt.z;

						index += 3;
					}
				}
			}

			void getPatch(int patchNum, glm::vec3 patch[][4], bool reverseV)
			{
				for (int u = 0; u < 4; u++) {          // Loop in u direction
					for (int v = 0; v < 4; v++) {     // Loop in v direction
						if (reverseV) {
							patch[u][v] = glm::vec3(
								cpdata[patchdata[patchNum][u * 4 + (3 - v)]][0],
								cpdata[patchdata[patchNum][u * 4 + (3 - v)]][1],
								cpdata[patchdata[patchNum][u * 4 + (3 - v)]][2]
								);
						}
						else {
							patch[u][v] = glm::vec3(
								cpdata[patchdata[patchNum][u * 4 + v]][0],
								cpdata[patchdata[patchNum][u * 4 + v]][1],
								cpdata[patchdata[patchNum][u * 4 + v]][2]
								);
						}
					}
				}
			}

		};
	}
}