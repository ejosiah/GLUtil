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
#include "textures.h"
#include "WithTriangleAdjacency.h"

#pragma comment(lib, "assimp-vc140-mt.lib")
//#pragma comment(lib, "assimp-vc142-mtd.lib")

namespace ncl {
	namespace gl {
		class Model : public Shape, public WithTriangleAdjacency {

		public:
			static const unsigned int DEFAULT_PROCESS_FLAGS = aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;

			Model(std::string path, bool normalize = false, float scale = 1, bool adjacency = false, unsigned int pFlags = DEFAULT_PROCESS_FLAGS):
				Shape(createMesh(path, normalize, scale, pFlags, adjacency)){}
			
			Bounds* bound;

			void initBounds() {
				bounds.max = bounds.max = bounds.max = glm::vec3(0);
				bounds.min = bounds.min = bounds.min = glm::vec3(0);

				bounds.min.x = bounds.min.y = bounds.min.z =  std::numeric_limits<float>::max();
				bounds.max.x = bounds.max.y = bounds.max.z = std::numeric_limits<float>::min();
			}

			 std::vector<Mesh> createMesh(std::string path, bool _normalize, float scale, unsigned pFlags, bool adjacency) {
				initBounds();
				std::vector<Mesh> meshes;
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(path, pFlags);
				int i = path.find_last_of("\\") + 1;
				std::string parent = path.substr(0, i);
				extractMesh(scene->mRootNode, scene, meshes, parent);
				if (_normalize) {
					normalize(meshes, scale);
				}
				calculateBounds(meshes);
				bound = new Bounds(mesurements = { bounds.min, bounds.max });
				Logger logger = Logger::get("Model");
				size_t vertices = 0;
				std::for_each(meshes.begin(), meshes.end(), [&](Mesh& m) {  vertices += m.positions.size();});
				logger.info("no of vertices loaded: " + std::to_string(vertices));
				if (adjacency) {
					logger.info("added adjacency data to mesh");
					std::for_each(meshes.begin(), meshes.end(), [&](Mesh& m) {  
						m.indices = addAdjacency(m.indices);
						m.primitiveType = GL_TRIANGLES_ADJACENCY;
					});
				}
				return meshes;
			}

			void extractMesh(const aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, std::string& parent) {
				using namespace std;
				for (unsigned i = 0; i < node->mNumMeshes; i++) {
					aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
					Mesh mesh;
					mesh.name = aiMesh->mName.C_Str();
					mesh.primitiveType = toGL(aiMesh->mPrimitiveTypes);
					for (unsigned j = 0; j < aiMesh->mNumVertices; j++) {
						aiVector3D aiv = aiMesh->mVertices[j];


						glm::vec3 pos{ aiv.x, aiv.y, aiv.z };

						if (pos.x < bounds.min.x) {
							bounds.min.x = pos.x;
						}
						if (pos.y < bounds.min.y) {
							bounds.min.y = pos.y;
						}
						if (pos.z < bounds.min.z) {
							bounds.min.z = pos.z;
						}

						if (pos.x > bounds.max.x) {
							bounds.max.x = pos.x;
						}
						if (pos.y > bounds.max.y) {
							bounds.max.y = pos.y;
						}
						if (pos.z > bounds.max.z) {
							bounds.max.z = pos.z;
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
					Material material;
					float data[3];
					unsigned int size = 3;

					aiString name;
					aiReturn ret = aiMaterial->Get(AI_MATKEY_NAME, name);
					if (ret == aiReturn_SUCCESS) {
						material.name = name.C_Str();
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, data, &size);
					float opacity = 1;
					ret = aiMaterial->Get(AI_MATKEY_OPACITY, opacity);
					if (ret == aiReturn_SUCCESS) {
						material.opacity = opacity;
					}
					if (ret == aiReturn_SUCCESS) {
						material.diffuse = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, data, &size);
					if (ret == aiReturn_SUCCESS) {
						material.ambient = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, data, &size);
					if (ret == aiReturn_SUCCESS) {
						material.specular = { data[0], data[1], data[2], opacity };
					}

					ret = aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, data, &size);
					if (ret == aiReturn_SUCCESS) {
						material.emission = { data[0], data[1], data[2], opacity };
					}

					float f = 0;

					ret = aiMaterial->Get(AI_MATKEY_REFRACTI, f);
					if (ret == aiReturn_SUCCESS) {
						material.ior = f;
					}

					ret = aiMaterial->Get(AI_MATKEY_SHININESS, f);
					if (ret == aiReturn_SUCCESS) {
						material.shininess = f;
					}

					aiString path;
					ret = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
					if (ret == aiReturn_SUCCESS) {
						string texPath = parent + path.C_Str();
						diffuse = new Texture2D(texPath, 1);
						material.diffuseMat = diffuse->buffer();
						material.diffuseTexPath = texPath;
						material.usingDefaultMat = false;
					}
					else {
						diffuse = new CheckerTexture(0, "diffuseMap", material.diffuse, material.diffuse);
						material.diffuseMat = diffuse->buffer();
						material.usingDefaultMat = false;
					}

					ret = aiMaterial->GetTexture(aiTextureType_AMBIENT, 0, &path);
					if (ret == aiReturn_SUCCESS) {
						string texPath = parent + path.C_Str();
						ambient = new Texture2D(texPath, 0);
						material.ambientMat = ambient->buffer();
						material.ambientTexPath = texPath;
					}
					else if (material.diffuseMat > -1) {
						material.ambientMat = material.diffuseMat;
						material.ambientTexPath = material.diffuseTexPath;
					}
					else {
						ambient = new CheckerTexture(0, "ambientMap", material.ambient, material.ambient);
						material.ambientMat = ambient->buffer();
					}

					ret = aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path);
					if (ret == aiReturn_SUCCESS) {
						string texPath = parent + path.C_Str();
						specular = new Texture2D(texPath, 2);
						material.specularMat = diffuse->buffer();
						material.specularTexPath = texPath;
					}

					ret = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &path);
					if (ret == aiReturn_SUCCESS) {
						string texPath = parent + path.C_Str();
						normal = new Texture2D(texPath, 4);
						material.bumpMap = diffuse->buffer();
						material.bumpTexPath = texPath;
					}

					ret = aiMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &path);
					if (ret == aiReturn_SUCCESS) {
						string texPath = parent + path.C_Str();
						normal = new Texture2D(texPath, 4);
						material.bumpMap = diffuse->buffer();
						material.bumpTexPath = texPath;
					}

					mesh.material = material;
					meshes.push_back(mesh);
				}

				for (unsigned i = 0; i < node->mNumChildren; i++) {
					extractMesh(node->mChildren[i], scene, meshes, parent);
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


			std::vector<Mesh> extractMesh(const aiScene* scene, std::string parent) {
				std::vector<Mesh> meshes;

				extractMesh(scene->mRootNode, scene, meshes, parent);

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
							if (pos.x < bounds.min.x) {
								bounds.min.x = pos.x;
							}
							if (pos.y < bounds.min.y) {
								bounds.min.y = pos.y;
							}
							if (pos.z < bounds.min.z) {
								bounds.min.z = pos.z;
							}

							if (pos.x > bounds.max.x) {
								bounds.max.x = pos.x;
							}
							if (pos.y > bounds.max.y) {
								bounds.max.y = pos.y;
							}
							if (pos.z > bounds.max.z) {
								bounds.max.z = pos.z;
							}
						}
					}
				}

			protected:
				glm::vec3 center;
				Mesurements mesurements;
				struct {
					glm::vec3 min, max;
				} bounds;
				Texture2D* ambient;
				Texture2D* diffuse;
				Texture2D* specular;
				Texture2D* normal;

		};
	}
}