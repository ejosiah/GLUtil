#pragma once

#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {
		class CopyBuffer {
		public:
			GLuint copy(GLuint bufferId) {
				GLint size;
				glBindBuffer(GL_COPY_READ_BUFFER, bufferId);
				glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

				GLuint copyBufferId;
				glGenBuffers(1, &copyBufferId);
				glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferId);
				glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

					return copyBufferId;
			}
		};
	}
}
