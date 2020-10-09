#pragma once

#include <tuple>
#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {

		constexpr GLuint Size = 0;
		constexpr GLuint Buffer = 1;

		class CopyBuffer {
		public:
			std::tuple<GLuint, GLuint> copy(GLuint bufferId) {
				GLint size;
				glBindBuffer(GL_COPY_READ_BUFFER, bufferId);
				glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

				GLuint copyBufferId;
				glGenBuffers(1, &copyBufferId);
				glBindBuffer(GL_COPY_WRITE_BUFFER, copyBufferId);
				glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

				return std::make_tuple(size, copyBufferId);
			}
		};
	}
}
