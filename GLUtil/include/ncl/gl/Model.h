#pragma once

#include <functional>
#include <numeric>
#include <algorithm>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <gl/gl_core_4_5.h>
#include "Shape.h"
#include "Bounds.h"

#pragma comment(lib, "assimp-vc140-mt.lib")

namespace ncl {
	namespace gl {
		class Model : public Shape {

		public:
			Model(std::string path, bool normalize = false, float scale = 1, unsigned int pFlags = DEFAULT_PROCESS_FLAGS):
				Shape(createMesh(path, normalize, scale, pFlags)){}

			void initBounds() {
				bounds.maxX = bounds.maxY = bounds.maxZ = glm::vec3(0);
				bounds.minX = bounds.minY = bounds.minZ = glm::vec3(0);

				bounds.minX.x = bounds.minY.y = bounds.minZ.z =  std::numeric_limits<float>::max();
				bounds.maxX.x = bounds.maxY.y = bounds.maxZ.z = std::numeric_limits<float>::min();
			}

			 std::vector<Mesh> createMesh(std::string path, bool _normalize, float scale, unsigned pFlags) {
				initBounds();
				std::vector<Mesh> meshes;
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(path, pFlags);
				extractMesh(scene->mRootNode, scene, meshes);
				if (_normalize) {
					normalize(meshes, scale);
				}
				calculateBounds(meshes);
				bound = new Bounds(mesurements = { bounds.minX, bounds.minY, bounds.minZ, bounds.maxX, bounds.maxY, bounds.maxZ });
				return meshes;
			}

			void extractMesh(const aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes) {
				for (unsigned i = 0; i < node->mNumMeshes; i++) {
					aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
					Mesh mesh;
					mesh.primitiveType = aiMesh->mPrimitiveTypes;
					for (unsigned j = 0; j < aiMesh->mNumVertices; j++) {
						aiVector3D aiv = aiMesh->mVertices[j];


						glm::vec3 pos{ aiv.x, aiv.y, aiv.z };

						if (pos.x < bounds.minX.x) {
							bounds.minX.x = pos.x;
						}
						if (pos.y < bounds.minY.y) {
							bounds.minY.y = pos.y;
						}
						if (pos.z < bounds.minZ.z) {
							bounds.minZ.z = pos.z;
						}

						if (pos.x > bounds.maxX.x) {
							bounds.maxX.x = pos.x;
						}
						if (pos.y > bounds.maxY.y) {
							bounds.maxY.y = pos.y;
						}
						if (pos.z > bounds.maxZ.z) {
							bounds.maxZ.z = pos.z;
						}

						mesh.positions.push_back(pos);


						if (aiMesh->HasNormals()) {
							aiVector3D aivn = aiMesh->mNormals[j];
							mesh.normals.push_back(glm::vec3{ aivn.x, aivn.y, aivn.z });
						}

						if (aiMesh->HasTangentsAndBitangents()) {
							aiVector3D t = aiMesh->mTangents[j];
							aiVector3D bt = aiMesh->mBitangents[j];
							mesh.tangents.push_back(glm::vec3{ t.x, t.y, t.z });
							mesh.bitangents.push_back(glm::vec3{ bt.x, bt.y, bt.z });
						}
						else {
							mesh.tangents.push_back(glm::vec3{0});
							mesh.bitangents.push_back(glm::vec3{0});
						}

						if (aiMesh->HasVertexColors(0)) {
							aiColor4D aic = aiMesh->mColors[0][j];
							mesh.colors.push_back(glm::vec4{ aic.r, aic.g, aic.b, aic.a });
						}

						for (int k = 0; k < AI_MAX_NUMBER_OF_TEXTURECOORDS; k++) {
							if (aiMesh->HasTextureCoords(k)) {
								aiVector3D aitx = aiMesh->mTextureCoords[k][j];
								mesh.uvs[k].push_back(glm::vec2{ aitx.x, aitx.y });
							}
						}

					}

					if (aiMesh->HasFaces()) {
						for (unsigned j = 0; j < aiMesh->mNumFaces; j++) {
							aiFace face = aiMesh->mFaces[j];
							for (unsigned k = 0; k < face.mNumIndices; k++) {
								mesh.indices.push_back(face.mIndices[k]);
							}
						}
					}

					aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];
					Material mat;
					float data[3];
					unsigned int size = 3;

					aiReturn ret = aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, data, &size);
					float opacity = 1;
					ret = aiMaterial->Get(AI_MATKEY_OPACITY, opacity);
					if (ret == aiReturn_SUCCESS) {
						mat.opacity = opacity;
					}
					if (ret == aiReturn_SUCCESS) {
						mat.diffuse = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, data, &size);
					if (ret == aiReturn_SUCCESS) {
						mat.ambient = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, data, &size);
					if (ret == aiReturn_SUCCESS) {
						mat.specular = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, data, &size);
					if (ret == aiReturn_SUCCESS) {
						mat.emission = { data[0], data[1], data[2], opacity };
					}

					float f = 0;

					ret = aiMaterial->Get(AI_MATKEY_REFRACTI, f);
					if (ret == aiReturn_SUCCESS) {
						mat.ior = f;
					}

					ret = aiMaterial->Get(AI_MATKEY_SHININESS, f);
					if (ret == aiReturn_SUCCESS) {
						mat.shininess = f;
					}

					mesh.material = mat;
					meshes.push_back(mesh);
				}

				for (unsigned i = 0; i < node->mNumChildren; i++) {
					extractMesh(node->mChildren[i], scene, meshes);
				}

			}

			GLenum toGL(unsigned int type) {
				switch (type) {
				case aiPrimitiveType_POINT: return GL_POINTS;
				case aiPrimitiveType_LINE: return GL_LINES;
				case aiPrimitiveType_TRIANGLE: return GL_TRIANGLES;
				default:
					throw "unknow primitive type";
				}
			}


			std::vector<Mesh> extractMesh(const aiScene* scene) {
				std::vector<Mesh> meshes;

				extractMesh(scene->mRootNode, scene, meshes);

				std::sort(meshes.begin(), meshes.end(), [](Mesh& a, Mesh& b) {
					return a.material.opacity >= b.material.opacity;
				});

				return meshes;
			}

			virtual void drawBounds(Shader& shader) {
				GLint mode;
				glGetIntegerv(GL_POLYGON_MODE, &mode);

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				bound->draw(shader);
				glPolygonMode(GL_FRONT_AND_BACK, mode);
			}

			float height() const {
				return mesurements.height;
			}

			float width() const {
				return mesurements.width;
			}

			float length() const {
				return mesurements.length;
			}

			void forEachMaterial(std::function<void(Material&)> consume) {
				for (Material& m : materials) {
					consume(m);
				}
			}
			
			private:
				void calculateBounds(std::vector<Mesh>& meshes) {
					initBounds();
					for (Mesh& mesh : meshes) {
						for (glm::vec3& pos : mesh.positions) {
							if (pos.x < bounds.minX.x) {
								bounds.minX.x = pos.x;
							}
							if (pos.y < bounds.minY.y) {
								bounds.minY.y = pos.y;
							}
							if (pos.z < bounds.minZ.z) {
								bounds.minZ.z = pos.z;
							}

							if (pos.x > bounds.maxX.x) {
								bounds.maxX.x = pos.x;
							}
							if (pos.y > bounds.maxY.y) {
								bounds.maxY.y = pos.y;
							}
							if (pos.z > bounds.maxZ.z) {
								bounds.maxZ.z = pos.z;
							}
						}
					}
				}

			protected:
				static const unsigned int DEFAULT_PROCESS_FLAGS = aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
				Bounds* bound;
				glm::vec3 center;
				Mesurements mesurements;
				struct {
					glm::vec3 minX, minY, minZ, maxX, maxY, maxZ;
				} bounds;
		};
	}
}