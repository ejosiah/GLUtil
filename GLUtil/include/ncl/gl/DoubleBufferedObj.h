#pragma once

#include <initializer_list>
#include <functional>
#include "Shape.h"
#include "textures.h"

namespace ncl {
	namespace gl {
		class DoubleBufferedObj {
		public:
			DoubleBufferedObj(Shape* shape, std::initializer_list<int> attributes){
				for (auto attribute : attributes) {
					GLintptr write_offset = 0;
					GLuint copyBufferIds[2];
					glGenBuffers(2, copyBufferIds);
					for (int i = 0; i < shape->numMeshes(); i++) {
						GLuint bufferId = shape->getBuffers()[i][attribute];
						GLint size = 0;

						glBindBuffer(GL_COPY_READ_BUFFER, bufferId);
						glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

						glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferIds[0]);
						glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
						glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, write_offset, size);

						glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferIds[1]);
						glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
						glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, write_offset, size);
						write_offset += size;
					}
					_buffers.push_back(new DoubleBuffer(attribute, GL_RGBA32F, copyBufferIds));
				}
				
				glGenVertexArrays(2, vaoIds);

				for (int i = 0; i < 2; i++) {
					glBindVertexArray(vaoIds[i]);
					GLuint bufId = _buffers[0]->buffer(i);
					glBindBuffer(GL_ARRAY_BUFFER, bufId);
					glEnableVertexAttribArray(VAOObject::Position);
					glVertexAttribPointer(VAOObject::Position, 3, GL_FLOAT, GL_FALSE, 0, 0);

					if (attributes.size() > 1 && shape->normals[0]) {
						bufId = _buffers[1]->buffer(i);
						glBindBuffer(GL_ARRAY_BUFFER, bufId);
						glEnableVertexAttribArray(VAOObject::Normal);
						glVertexAttribPointer(VAOObject::Normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
					}
					glBindVertexArray(0);
				}
			}

			void swapBuffers() {
				for (auto buffer : _buffers) {
					buffer->swapBuffers();
				}
			}

			void activate() {
				for (auto buffer : _buffers) {
					buffer->activate();
				}
			}

			void use(std::function<void(GLuint*, GLsizei)> proc) {
				activate();
				glBindVertexArray(_buffers[0]->front());

				std::vector<GLuint> bufIds;
				for (auto buffer : _buffers) {
					bufIds.push_back(buffer->buffer(buffer->back()));
				}

				proc(&bufIds[0], bufIds.size());

				swapBuffers();
				glBindVertexArray(0);
			}

		private:
			std::vector<DoubleBuffer*> _buffers;
			GLenum primitiveType;
			GLuint vaoIds[2];
		};
	}
}