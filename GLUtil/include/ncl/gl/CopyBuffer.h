#pragma once

#include <tuple>
#include <gl/gl_core_4_5.h>
#include <algorithm>

namespace ncl {
	namespace gl {

		constexpr GLuint Size = 0;
		constexpr GLuint Buffer = 1;

		class CopyBuffer {
		public:
			std::tuple<GLuint, GLuint> copy(GLuint bufferId, GLuint copyBufferId = 0, GLint copy_size = 0) {
				GLint size;
				glBindBuffer(GL_COPY_READ_BUFFER, bufferId);
				glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

				if (copy_size != 0) {
					size = std::min(copy_size, size);
				}

				if (!glIsBuffer(copyBufferId)) {
					glGenBuffers(1, &copyBufferId);
					glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferId);
					glBufferData(GL_COPY_WRITE_BUFFER, std::max(copy_size, size), nullptr, GL_DYNAMIC_COPY);
				}
				else {
					glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferId);
				}
				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

				return std::make_tuple(copy_size, copyBufferId);
			}
		};
	}
}
