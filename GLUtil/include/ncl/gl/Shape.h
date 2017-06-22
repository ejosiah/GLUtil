#pragma once
#include "VAOObject.h"
#include "Drawable.h"
#include <functional>
#include <algorithm>
#include <set>

#include "common.h"

namespace ncl {
	namespace gl {
		class Shape : public VAOObject, public Drawable {
		public:
			Shape(std::vector<Mesh> meshes, bool cullface = true) 
				:VAOObject(meshes)
				, cullface(cullface) {

			}

			virtual void draw(Shader& shader) override {
				bool cullingDisabled = false;
				if (!cullface && glIsEnabled(GL_CULL_FACE)) {
					cullingDisabled = true;
					glDisable(GL_CULL_FACE);
				}
				const int num_vaos = vaoIds.size();
				for (int i = 0; i < num_vaos; i++) {
					GLuint vaoId = vaoIds[i];
					glBindVertexArray(vaoId);

					shader.sendUniformMaterial("material[0]", materials[i]);

					if (!indices[i]) {
						glDrawArrays(primitiveType[i], 0, counts[i]);
					}
					else {
						glDrawElements(primitiveType[i], counts[i], GL_UNSIGNED_INT, 0);
					}
					glBindVertexArray(0);
				}
				if (cullingDisabled) glEnable(GL_CULL_FACE);
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
			}

			template<typename T>
			void get(int attribute, std::function<void(T*)> use) const {
				if (attribute < Position || attribute > Indices)
					throw std::runtime_error("invalid attribute id");
				glBindVertexArray(vaoIds[0]);
				int bufferId = 0;
				switch (attribute) {
				case Position:
					break;
				case Indices:
					bufferId = numBuffers - 1;
					break;
				default:
					for (int i = 0; i < attribute; i++) {
						if (attributes[0][i]) {
							bufferId++;
						}
					}
				}
				T* data = (T*)glMapNamedBuffer(buffers[0][bufferId], GL_READ_ONLY);
				use(data);
				glUnmapNamedBuffer(buffers[0][bufferId]);
			}


			int numVertices(int meshId) const {
				if (!indices[meshId]) {
					return counts[meshId];
				}
				else {
					std::set<GLuint> indices;
					int size = counts[meshId];
					get<GLuint>(Indices, [&](GLuint* index) {
						GLuint* begin = index;
						GLuint* end = index + size;
						for (; index != end; index++) {
							indices.insert(*index);
						}
					});
					return indices.size();
				}
			}

			Material& material() {
				return materials[0];
			}

		protected:
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

		private:
			bool cullface;
		};
	}
}