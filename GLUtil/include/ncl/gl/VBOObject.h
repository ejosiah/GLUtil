#pragma once

#include <algorithm>
#include <vector>
#include <array>
#include <gl/gl_core_4_5.h>
#include "mesh.h"
#include "Drawable.h"
#include "common.h"

namespace ncl {
	namespace gl {

		class VBOObject{
		public:
			VBOObject(std::vector<Mesh>& meshes) {
				int no_of_meshes = meshes.size();
				normals = std::vector<bool>(no_of_meshes, false);
				colors = std::vector<bool>(no_of_meshes, false);
				tangents = std::vector<bool>(no_of_meshes, false);
				for (std::vector<bool>& txc : texCoords) {
					txc = std::vector<bool>(no_of_meshes, false);
				}
				indices = std::vector<bool>(no_of_meshes, false);
				int cololBuffer = 1;
				numBuffers = calculateNoOfBuffers(meshes[0]) + cololBuffer;

				for (int i = 0; i < no_of_meshes; i++) {
					Mesh mesh = meshes[i];
					GLsizei size = mesh.positions.size();
					GLsizei count = mesh.indices.size();

					if (mesh.colors.empty()) {
						size_t n = mesh.positions.size();
						mesh.colors = std::vector<glm::vec4>(n, mesh.material.diffuse);
					}


					GLuint* buffer = new GLuint[numBuffers];
					GLuint* nextBuffer = buffer;
					std::vector<bool> attribs = std::vector<bool>(4, false);

					glGenBuffers(numBuffers, buffer);

					glBindBuffer(GL_ARRAY_BUFFER, *nextBuffer);
					glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.positions), &mesh.positions[0], GL_STATIC_DRAW);

					if (mesh.hasNormals()) {
						normals[i] = true;
						attribs[0] = true;
						glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.normals), &mesh.normals[0], GL_STATIC_DRAW);
					}


					if (mesh.hasTangents()) {
						tangents[i] = true;
						attribs[1] = true;
						glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.tangents), &mesh.tangents[0], GL_STATIC_DRAW);


						attribs[2] = true;
						glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.bitangents), &mesh.bitangents[0], GL_STATIC_DRAW);

					}


					if (mesh.hasColors()) {
						colors[i] = true;
						attribs[3] = true;
						glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.colors), &mesh.colors[0], GL_STATIC_DRAW);
					}


					if (mesh.hasTexCoords()) {
						const int numTextures = mesh.numTexCoords();
						for (int j = 0; j < numTextures; j++) {
							if (!mesh.uvs[j].empty()) {
								texCoords[j][i] = true;
								glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
								glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.uvs[j]), &mesh.uvs[j][0], GL_STATIC_DRAW);
							}
						}

					}


					if (mesh.hasIndices()) {
						indices[i] = true;
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeof(mesh.indices), &mesh.indices[0], GL_STATIC_DRAW);
					}


					buffers.push_back(buffer);
					materials.push_back(mesh.material);
					counts.push_back(count > 0 ? count : size);
					primitiveType.push_back(mesh.primitiveType);
					attributes.push_back(attribs);

				}
			}

			virtual ~VBOObject() {
				for (GLuint* buffer : buffers) {
					glDeleteBuffers(numBuffers, buffer);
					delete[] buffer;
				}
			}


			int calculateNoOfBuffers(Mesh& mesh) {
				int numBuffers = 1;
				if (mesh.hasNormals()) {
					numBuffers += 1;
				}
				if (mesh.hasTangents()) {
					numBuffers += 2;
				}
				if (mesh.hasColors()) {
					numBuffers += 1;
				}
				if (mesh.hasTexCoords()) {	
					for (int i = 0; i < MAX_UVS; i++) {
						if (!mesh.uvs[i].empty())
							numBuffers += 1;
					}
				}
				if (mesh.hasIndices()) {
					numBuffers += 1;
				}
				return numBuffers;
			}

			std::vector<GLuint*> getBuffers() const {
				return buffers;
			}

		protected:
			std::vector<Material> materials;
			std::vector<GLsizei> counts;
			std::vector<GLuint*> buffers;
			std::vector<GLenum> primitiveType;
			std::vector<bool> normals;
			std::vector<bool> colors;
			std::vector<bool> tangents;
			std::vector<std::vector<bool>> attributes;
			std::array<std::vector<bool>, MAX_UVS> texCoords;
			std::vector<bool> indices;
			int numBuffers = 0;

			template<typename T>
			GLsizei _sizeof(std::vector<T> vector) {
				return sizeof(vector[0]) * vector.size();
			}

		};
	}
}