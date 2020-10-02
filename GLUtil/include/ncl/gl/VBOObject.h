#pragma once

#include <algorithm>
#include <vector>
#include <array>
#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include "mesh.h"
#include "Drawable.h"
#include "common.h"
#include "CopyBuffer.h"

namespace ncl {
	namespace gl {

		class VBOObject : public CopyBuffer{
		public:
			VBOObject() = default;

			VBOObject(std::vector<Mesh>& meshes, unsigned int instanceCount) {
				for (auto& mesh : meshes) {
					if (!mesh.hasXforms() || mesh.xforms.size() < instanceCount)
						mesh.xforms = std::vector<glm::mat4>{ instanceCount, glm::mat4(1) };
				}
				int no_of_meshes = meshes.size();
				normals = std::vector<bool>(no_of_meshes, false);
				colors = std::vector<bool>(no_of_meshes, false);
				tangents = std::vector<bool>(no_of_meshes, false);
				for (std::vector<bool>& txc : texCoords) {
					txc = std::vector<bool>(no_of_meshes, false);
				}
				xforms = std::vector<bool>(no_of_meshes, false);
				indices = std::vector<bool>(no_of_meshes, false);
				numBuffers = calculateNoOfBuffers(meshes[0]);

				for (int i = 0; i < no_of_meshes; i++) {
					Mesh mesh = meshes[i];
					GLsizei size = mesh.positions.size();
					GLsizei count = mesh.indices.size();

					if (mesh.colors.empty()) {
						size_t n = mesh.positions.size();
						mesh.colors = std::vector<glm::vec4>(n, mesh.material.diffuse);
						numBuffers++;
					}

					GLuint* buffer = new GLuint[numBuffers];
					GLuint* nextBuffer = buffer;
					std::vector<bool> attribs = std::vector<bool>(4, false);

					glGenBuffers(numBuffers, buffer);

					glBindBuffer(GL_ARRAY_BUFFER, *nextBuffer);
					glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.positions), &mesh.positions[0], GL_STATIC_DRAW);
				//	glBufferData(GL_ARRAY_BUFFER, _sizeof(positions), &positions[0], GL_STATIC_DRAW);

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

					if (mesh.hasXforms()) {
						xforms[i] = true;
						glBindBuffer(GL_ARRAY_BUFFER, *(++nextBuffer));
						glBufferData(GL_ARRAY_BUFFER, _sizeof(mesh.xforms), &mesh.xforms[0], GL_STATIC_DRAW);
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

			VBOObject(VBOObject&& source) noexcept {
				transfer(source, *this);
			}

			VBOObject(const VBOObject&) = delete;

			virtual ~VBOObject() {
				for (GLuint* buffer : buffers) {
					glDeleteBuffers(numBuffers, buffer);
					delete[] buffer;
				}
			}

			VBOObject& operator=(VBOObject&& source) noexcept {
				transfer(source, *this);

				return *this;
			}

			VBOObject& operator=(const VBOObject&) = delete;

			void transfer(VBOObject& source, VBOObject& dest) {
				dest.materials = std::move(source.materials);
				dest.counts = std::move(source.counts);
				dest.buffers = std::move(source.buffers);
				dest.primitiveType = std::move(source.primitiveType);
				dest.normals = std::move(source.normals);
				dest.colors = std::move(source.colors);
				dest.tangents = std::move(source.tangents);
				dest.xforms = std::move(source.xforms);
				dest.attributes = std::move(source.attributes);
				dest.texCoords = std::move(source.texCoords);
				dest.indices = std::move(source.indices);
				dest.numBuffers = source.numBuffers;
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
				if (mesh.hasXforms()) {
					numBuffers += 1;
				}
				return numBuffers;
			}

			std::vector<GLuint*> getBuffers() const {
				return buffers;
			}

			bool hasNormals() const {
				return normals[0];
			}

			bool hasIndices() const {
				return indices[0];
			}

		protected:
			std::vector<Material> materials;
			std::vector<GLsizei> counts;
			std::vector<GLuint*> buffers;
			std::vector<GLenum> primitiveType;
			std::vector<bool> normals;
			std::vector<bool> colors;
			std::vector<bool> tangents;
			std::vector<bool> xforms;
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