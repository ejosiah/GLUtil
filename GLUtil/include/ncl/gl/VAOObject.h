#pragma once

#include <glm/glm.hpp>
#include "Drawable.h"
#include "VBOObject.h"
#include <functional>
#include <GLFW/glfw3.h>
#include <map>

namespace ncl {
	namespace gl {
		class VAOObject : public VBOObject {	
		public:
			// XForm = TexCoord + MAX_UVS + 1;
			enum BufferIds { Position, Normal, Tangent, BiTangent, Color, TexCoord, Indices, XForms = 8};
			friend class DoubleBufferedObj;

			VAOObject() = default;

			VAOObject(std::vector<Mesh>& meshes, unsigned int instanceCount)
				:VBOObject(meshes, instanceCount)
				, currentContext{ glfwGetCurrentContext() } {
				vaoIds = init();
			}

			std::vector<GLuint> init() {
				const int no_of_vaos = buffers.size();

				vaoIds = std::vector<GLuint>(no_of_vaos);
				glGenVertexArrays(no_of_vaos, &vaoIds[0]);


				for (int i = 0; i < no_of_vaos; i++) {
					GLuint* buffer = buffers[i];


					glBindVertexArray(vaoIds[i]);
					glBindBuffer(GL_ARRAY_BUFFER, *buffer);
					glEnableVertexAttribArray(Position);
					glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
				//	glVertexAttribPointer(Position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

					if (normals[i]) {
						glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
						glEnableVertexAttribArray(Normal);
						glVertexAttribPointer(Normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
					}

					if (tangents[i]) {
						glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
						glEnableVertexAttribArray(Tangent);
						glVertexAttribPointer(Tangent, 3, GL_FLOAT, GL_FALSE, 0, 0);

						glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
						glEnableVertexAttribArray(BiTangent);
						glVertexAttribPointer(BiTangent, 3, GL_FLOAT, GL_FALSE, 0, 0);
					}

					if (colors[i]) {
						glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
						glEnableVertexAttribArray(Color);
						glVertexAttribPointer(Color, 4, GL_FLOAT, GL_FALSE, 0, 0);
					}

					for (int j = 0; j < MAX_UVS; j++) {
						if (texCoords[j][i]) {
							glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
							glEnableVertexAttribArray(TexCoord + j);
							glVertexAttribPointer(TexCoord + j, 2, GL_FLOAT, GL_FALSE, 0, 0);
						}
					}
					if (xforms[i]) {
						glBindBuffer(GL_ARRAY_BUFFER, *(++buffer));
						for (int i = 0; i < 4; i++) {
							glEnableVertexAttribArray(XForms + i);
							glVertexAttribPointer(XForms + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4) * i)));
							glVertexAttribDivisor(XForms + i, 1);
						}
					}

					if (indices[i]) {
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(++buffer));
					}
					glBindVertexArray(0);
				}
				contextVAOIds[glfwGetCurrentContext()] = vaoIds;
				return vaoIds;
			}

			void ensureContext() {
				if (contextVAOIds.find(glfwGetCurrentContext()) == contextVAOIds.end()) {
					init();
				}
			}

			VAOObject(const VAOObject&) = delete;

			VAOObject(VAOObject&& source) noexcept{
				transfer(source, *this);
			}

			VAOObject& operator=(const VAOObject&) = delete;
			
			VAOObject& operator=(VAOObject&& source) noexcept {
				transfer(source, *this);
				return *this;
			}

			void transfer(VAOObject& source, VAOObject& dest) {
				VBOObject::transfer(dynamic_cast<VBOObject&>(source), dynamic_cast<VBOObject&>(dest));
				dest.vaoIds = std::move(source.vaoIds);
				dest.contextVAOIds = std::move(source.contextVAOIds);
			}

			void use(int vaoIndex, std::function<void()> proc) {
				glBindVertexArray(vaoIds[vaoIndex]);
				proc();
				glBindVertexArray(0);
			}

			virtual ~VAOObject() {
				if (!vaoIds.empty()) {
					glDeleteVertexArrays(vaoIds.size(), &vaoIds[0]);
				}
			}

			GLuint getVaoId(int index) {
				return vaoIds[index];
			}

		protected:
			std::vector<GLuint> vaoIds;
			std::map<GLFWwindow*, std::vector<GLuint>> contextVAOIds;
			GLFWwindow* currentContext;
		};
	}
}