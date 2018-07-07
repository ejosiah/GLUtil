#pragma once

#include <glm/glm.hpp>
#include "Drawable.h"
#include "VBOObject.h"
#include <functional>

namespace ncl {
	namespace gl {
		class VAOObject : public VBOObject {	
		public:
			enum BufferIds { Position, Normal, Tangent, BiTangent, Color, TexCoord, Indices };

			VAOObject(std::vector<Mesh>& meshes)
				:VBOObject(meshes) {
				const int no_of_vaos = buffers.size();
				
				vaoIds = std::vector<GLuint>(no_of_vaos);
				glGenVertexArrays(no_of_vaos, &vaoIds[0]);

			
				for (int i = 0; i < no_of_vaos; i++) {
					GLuint* buffer = buffers[i];


					glBindVertexArray(vaoIds[i]);
					glBindBuffer(GL_ARRAY_BUFFER, *buffer);
					glEnableVertexAttribArray(Position);
					glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

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

					if (indices[i]) {
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(++buffer));
					}
					glBindVertexArray(0);
				}
				
			}

			void use(int vaoIndex, std::function<void()> proc) {
				glBindVertexArray(vaoIds[vaoIndex]);
				proc();
				glBindVertexArray(0);
			}

			virtual ~VAOObject() {
				glDeleteVertexArrays(vaoIds.size(), &vaoIds[0]);
			}

		protected:
			std::vector<GLuint> vaoIds;
		};
	}
}