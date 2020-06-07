#pragma once

#include "VAOObject.h"
#include "Drawable.h"
#include <functional>
#include <algorithm>
#include <set>
#include <iterator>
#include "WithTriangleAdjacency.h"
#include "WithTangent.h"
#include "common.h"
#include "textures.h"
#include "TransformFeedBack.h"
#include "buffer_iterator.h"

namespace ncl {
	namespace gl {
		class Shape : public VAOObject, public Drawable {
		public:
			Shape() = default;

			Shape(std::vector<Mesh> meshes, bool cullface = true, unsigned instanceCount = 1) 
				:VAOObject(meshes)
				, cullface(cullface)
				, instanceCount(instanceCount)
				, tfb(nullptr){

				for (int i = 0; i < meshes.size(); i++) {
					meshName[meshes[i].name] = i;
				}
				whiteTexture = new CheckerTexture(0, "white", WHITE, WHITE);
				normalTexture = new CheckerTexture(0, "normalMap", BLUE, BLUE);
			}

			Shape(Shape&& source) noexcept {
				transfer(source, *this);
			}

			Shape(const Shape&) = delete;


			virtual ~Shape() {
				delete tfb; 
				delete whiteTexture;
				delete normalTexture;
			}


			Shape& operator=(const Shape&) = delete;

			Shape& operator=(Shape&& source) {
				transfer(source, *this);
				return *this;
			}
			
			virtual void draw(Shader& shader) override {
				ensureContext();
				bool cullingDisabled = false;
				if (!cullface && glIsEnabled(GL_CULL_FACE)) {
					cullingDisabled = true;
					glDisable(GL_CULL_FACE);
				}
				auto vaoIds = contextVAOIds[glfwGetCurrentContext()];
				const int num_vaos = vaoIds.size();
				for (int i = 0; i < num_vaos; i++) {
					GLuint vaoId = vaoIds[i];
					assert(glIsVertexArray(vaoId) == GL_TRUE);
					glBindVertexArray(vaoId);

					//if (i == 34) {
					//	shader.sendBool("isGround", true);
					//}
					//else {
					//	shader.sendBool("isGround", false);
					//}

					if (useDefaultMaterial) {
						Material& material = materials[i];
						shader.sendUniform1f("shininess", material.shininess);
						shader.sendUniform3fv("emission", 1, glm::value_ptr(material.emission));
						if (material.ambientMat != -1) {
							glBindTextureUnit(0, material.ambientMat);
						}
						//else {
						//	if (material.diffuseMat != -1) {
						//		glBindTextureUnit(0, material.diffuseMat);
						//	}
						//}
						if (material.diffuseMat != -1) {
							glBindTextureUnit(1, material.diffuseMat);
						}

						if (material.specularMat != -1) {
							glBindTextureUnit(2, material.specularMat);
						}
						else {
							glBindTextureUnit(2, whiteTexture->bufferId());
						}

						if (material.bumpMap != -1) {
							glBindTextureUnit(3, material.bumpMap);
						}
						else {
							glBindTextureUnit(3, normalTexture->bufferId());
						}

						shader.sendUniformMaterial("material[0]", material);

					}
					if (xforms[i]) {
						shader.sendBool("useXform", true);
					}
					
					if (tfb != nullptr) {
						shader.sendBool("capture", true);
						tfb->use({ captureBuffer }, 1, primitiveType[i], [&]() {
							drawPrimitive(i);
						});
					}
					else {
						drawPrimitive(i);
					}
					
					glBindVertexArray(0);
				}
				if (cullingDisabled) glEnable(GL_CULL_FACE);
				//(GL_TEXTURE0);
			}

			virtual void draw(Shader& shader, int meshId) {
				bool cullingDisabled = false;
				if (!cullface && glIsEnabled(GL_CULL_FACE)) {
					cullingDisabled = true;
					glDisable(GL_CULL_FACE);
				}
					GLuint vaoId = vaoIds[meshId];

					assert(glIsVertexArray(meshId) == GL_TRUE);

					glBindVertexArray(vaoId);

					if (useDefaultMaterial) {
						Material& material = materials[meshId];
						shader.sendUniform1f("shininess", material.shininess);
						shader.sendUniform3fv("emission", 1, glm::value_ptr(material.emission));
						if (material.ambientMat != -1) {
							assert(glIsTexture(material.ambientMat) == GL_TRUE);
							glBindTextureUnit(0, material.ambientMat);
						}
						//else {
						//	if (material.diffuseMat != -1) {
						//		glBindTextureUnit(0, material.diffuseMat);
						//	}
						//}
						if (material.diffuseMat != -1) {
							assert(glIsTexture(material.diffuseMat) == GL_TRUE);
							glBindTextureUnit(1, material.diffuseMat);
						}

						if (material.specularMat != -1) {
							glBindTextureUnit(2, material.specularMat);
						}
						else {
							glBindTextureUnit(2, whiteTexture->bufferId());
						}

						if (material.bumpMap != -1) {
							glBindTextureUnit(3, material.bumpMap);
						}
						else {
							glBindTextureUnit(3, normalTexture->bufferId());
						}

						shader.sendUniformMaterial("material[0]", material);

					}
					if (xforms[meshId]) {
						shader.sendBool("useXform", true);
					}

					if (tfb != nullptr) {
						shader.sendBool("capture", true);
						tfb->use({ captureBuffer }, 1, primitiveType[meshId], [&]() {
							drawPrimitive(meshId);
						});
					}
					else {
						drawPrimitive(meshId);
					}

					glBindVertexArray(0);
				if (cullingDisabled) glEnable(GL_CULL_FACE);
			}

			virtual void draw(Shader& shader, std::string mesh) {
				draw(shader, meshName[mesh]);
			}

			int getMeshId(std::string name) {
				if (meshName.find(name) != meshName.end()) {
					return meshName[name];
				}
				else {
					return -1;
				}
			}

			void drawPrimitive(int i) {
				if (!indices[i]) {
					if (instanceCount < 2) {
						glDrawArrays(primitiveType[i], 0, counts[i]);
					}
					else {
						glDrawArraysInstanced(primitiveType[i], 0, counts[i], instanceCount);
					}
				}
				else {
					if (instanceCount < 2) {
						glDrawElements(primitiveType[i], counts[i], GL_UNSIGNED_INT, 0);
					}
					else {
						glDrawElementsInstanced(primitiveType[i], counts[i], GL_UNSIGNED_INT, 0, instanceCount);
					}
				}
			}

			void enableTransformFeedBack(GLuint buf) {
				if (!xbfEnabled && tfb == nullptr) {
					captureBuffer = buf;
					tfb = new TransformFeebBack("xbf:shape" + std::to_string(buf), false);
				}
			}

			void disableTransformFeedBack() {
				xbfEnabled = false;
			}

			void update(int attribute, std::function<void(float*)> consume) {
				if (attribute < Position || attribute > Color)
					throw std::runtime_error("invalid attribute id");
				glBindVertexArray(vaoIds[0]);
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[0][i]) {
							bufferId++;
						}
					}
				}
				float* data = (float*)glMapNamedBuffer(buffers[0][bufferId], GL_READ_WRITE);
				consume(data);
				glUnmapNamedBuffer(buffers[0][bufferId]);
				glBindVertexArray(0);
			}

			template<typename T>
			void update2(int attribute, std::function<void(T*)> consume) {
				if (attribute < Position || attribute > Color)
					throw std::runtime_error("invalid attribute id");
				glBindVertexArray(vaoIds[0]);
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[0][i]) {
							bufferId++;
						}
					}
				}
				T* data = (T*)glMapNamedBuffer(buffers[0][bufferId], GL_READ_WRITE);
				consume(data);
				glUnmapNamedBuffer(buffers[0][bufferId]);
				glBindVertexArray(0);
			}

			GLuint bufferFor(int meshId, int attribute) const {
				if (attribute < Position || attribute > Indices)
					throw std::runtime_error("invalid attribute id");
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				case Indices:
					bufferId = numBuffers - 1;
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[meshId][i]) {
							bufferId++;
						}
					}
				}
				return buffers[meshId][bufferId];
			}


			template<typename T>
		//	[[deprecated("Replaced by get(unsigned int, int, std::function<void(buffer_iterator<true, T>)>), which has an improved interface")]]
			void get(unsigned int meshId, int attribute, std::function<void(T*)> use) const {
				// FIX might fail if we have multiple texture buffers
				if (attribute < Position || attribute > Indices)
					throw std::runtime_error("invalid attribute id");
				glBindVertexArray(vaoIds[meshId]);
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				case Indices:	
					bufferId = numBuffers - 1;
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[meshId][i]) {
							bufferId++;
						}
					}
				}
				T* data = (T*)glMapNamedBuffer(buffers[meshId][bufferId], GL_READ_ONLY);
				use(data);
				glUnmapNamedBuffer(buffers[meshId][bufferId]);
				glBindVertexArray(0);
			}

			template<typename T>
			void get(unsigned int meshId, int attribute, std::function<void(GlBuffer<T>)> use) const {
				auto vaoId = vaoIds[meshId];
				auto bufferId = bufferFor(meshId, attribute);
				use(GlBuffer<T>{ vaoId, bufferId });
			}

			 glm::vec3 getFirstVertex() const {
				return mapTo<glm::vec3, glm::vec3>(0, VAOObject::Position, [](glm::vec3* v) { return *v; });
			}

			template<typename R, typename T>
			R mapTo(unsigned int meshId, int attribute, std::function<R(T*)> mapper) const {
				// FIX might fail if we have multiple texture buffers
				if (attribute < Position || attribute > Indices)
					throw std::runtime_error("invalid attribute id");
				glBindVertexArray(vaoIds[meshId]);
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				case Indices:
					bufferId = numBuffers - 1;
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[meshId][i]) {
							bufferId++;
						}
					}
				}
				T* data = (T*)glMapNamedBuffer(buffers[meshId][bufferId], GL_READ_ONLY);
				R res = mapper(data);
				glUnmapNamedBuffer(buffers[meshId][bufferId]);
				glBindVertexArray(0);
				return res;
			}


			int numVertices(int meshId) const {
				GLint size;
				glBindBuffer(GL_ARRAY_BUFFER, buffers[meshId][Position]);
				glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				return size / sizeof(glm::vec3);
			}

			int numIndices(int meshId) const {
				if (!indices[meshId]) return 0;
				GLint size;
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferFor(meshId, Indices));
				glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				return size/sizeof(unsigned int);
			}

			std::vector<Mesh> getMeshes() {
				std::vector<Mesh> meshes;
				int no_of_meshes = vaoIds.size();
				for (int i = 0; i < no_of_meshes; i++) {
					Mesh mesh;
					int no_of_vertices = numVertices(i);
					get<glm::vec3>(i, Position, [&](glm::vec3* vertex) {
						glm::vec3* end = vertex + no_of_vertices;
						for (glm::vec3* next = vertex; next != end; next++) {
							mesh.positions.push_back(*next);
						}
					});
					if (normals[i]) {
						get<glm::vec3>(i, Normal, [&](glm::vec3* normal) {
							glm::vec3* end = normal + no_of_vertices;
							for (glm::vec3* next = normal; next != end; next++) {
								mesh.normals.push_back(*next);
							}
						});
					}
					if (colors[i]) {
						get<glm::vec4>(i, Color, [&](glm::vec4* color) {
							glm::vec4* end = color + no_of_vertices;
							for (glm::vec4* next = color; next != end; next++) {
								mesh.colors.push_back(*next);
							}
						});
					}
					if (indices[i]) {
						get<GLuint>(i, Indices, [&](GLuint* index) {
							GLuint* end = index + counts[i];
							for (GLuint* next = index; next != end; next++) {
								mesh.indices.push_back(*next);
							}
						});
					}
					mesh.material = materials[i];
					mesh.primitiveType = primitiveType[i];
					meshes.push_back(mesh);
				}
				return meshes;
			}

			int numVertices() const {
				int total = 0;
				size_t meshCount = numMeshes();
				for (int i = 0; i < meshCount; i++) {
					total += numVertices(i);
				}
				return total;
			}

			int numMeshes() const {
				return vaoIds.size();
			}

			int numTriangles() {
				if (primitiveType[0] != GL_TRIANGLES) return 0; // TODO handle other triangle types
				int n = 0;
				int v = 0;
				for (int i = 0; i < vaoIds.size(); i++) {
					n = numIndices(i);
					v = numVertices(i);
				}
				if (n > 0) return n / 3;
				return v / 3;
			}

			Material& material(int index = 0) {
				return materials[index];
			}

			void changePrimitiveType(GLenum newType) {
				primitiveType[0] = newType;
			}

			void defautMaterial(bool flag) {
				useDefaultMaterial = flag;
			}

		protected:
			Texture2D* checkerBoard;
			void normalize(std::vector<Mesh>& mesh, float _scale) {
				float radius;
				glm::vec3 center;

				calculateBounds(mesh, radius, center);

				float scalingFactor = _scale / radius;

				glm::vec3 offset = -center;
				scale(mesh, scalingFactor, offset);
			}

			void calculateBounds(std::vector<Mesh>& meshes, float& radius, glm::vec3& center) {
				int numVertices = 0;

				center = glm::vec3(0);
				radius = 0.0f;

				for (Mesh& mesh : meshes) {
					numVertices += mesh.positions.size();
					for (glm::vec3& position : mesh.positions) {
						center += position;
						float dSquared = squaredDistance(position);
						if (dSquared > radius) {
							radius = dSquared;
						}
					}
				}

				radius = sqrtf(radius);

				center *= (1.0f / numVertices);
			}

			void scale(std::vector<Mesh>& meshes, float& scalingFactor, glm::vec3& offset) {
				for (Mesh& mesh : meshes) {
					for (glm::vec3& position : mesh.positions) {
						position = (position + offset) * scalingFactor;
					}
				}
			}

			// move to util
			float squaredDistance(glm::vec3& v) {
				return v.x*v.x + v.y*v.y + v.z*v.z;
			}

			static std::vector<Mesh>& postProcess(std::vector<Mesh>& meshes) {

			}

			void transfer(Shape& source, Shape& dest) {

				VAOObject::transfer(dynamic_cast<VAOObject&>(source), dynamic_cast<VAOObject&>(dest));

				dest.cullface = source.cullface;
				dest.xbfEnabled = source.xbfEnabled;
				dest.instanceCount = source.instanceCount;
				dest.tfb = source.tfb;
				dest.captureBuffer = source.captureBuffer;
				dest.meshName = std::move(source.meshName);
				dest.whiteTexture = source.whiteTexture;
				dest.normalTexture = source.normalTexture;
				dest.useDefaultMaterial = source.useDefaultMaterial;

				source.whiteTexture = nullptr;
				source.normalTexture = nullptr;
				source.tfb = nullptr;
			}

		private:
			bool cullface;
			bool xbfEnabled = false;
			unsigned instanceCount;
			TransformFeebBack* tfb = nullptr;	
			GLuint captureBuffer;
			std::unordered_map<std::string, int> meshName;
			Texture2D* whiteTexture = nullptr;
			Texture2D* normalTexture = nullptr;
			bool useDefaultMaterial = true;
		};
	}
}