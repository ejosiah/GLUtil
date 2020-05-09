#pragma once

#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include <functional>

namespace ncl {
	namespace gl {

		class FrameBuffer {
		public:
			struct Config {
				GLsizei width;
				GLsizei height;
				GLenum fboTarget = GL_FRAMEBUFFER;
				GLenum texTarget = GL_TEXTURE_2D;
				GLint texLevel = 0;
				GLint internalFmt = GL_RGB;
				GLint fmt = GL_RGB;
				GLint border = 0;
				GLfloat borderColor[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
				GLenum type = GL_UNSIGNED_BYTE;
				GLenum magFilter = GL_LINEAR;
				GLenum minfilter = GL_LINEAR;
				GLenum wrap_s = GL_REPEAT;
				GLenum wrap_t = GL_REPEAT;
				GLenum wrap_r = GL_REPEAT;
				GLenum attachment = GL_COLOR_ATTACHMENT0;
				glm::vec4 clearColor = WHITE;
				bool depthTest = true;
				bool stencilTest = true;
				bool depthAndStencil = true;
				bool deleteTexture = true;
				bool read = true;
				bool write = true;
			};

			enum class Status{ Complete, UnComplete};

			FrameBuffer() = default;

			FrameBuffer(Config, std::function<void()> extraTexConfig = [] {});

			FrameBuffer(const FrameBuffer&) = delete;

			FrameBuffer(FrameBuffer&&) noexcept;

			~FrameBuffer();

			FrameBuffer& operator=(const FrameBuffer&) = delete;

			FrameBuffer& operator=(FrameBuffer&&) noexcept;

			inline bool rboBuf() const {
				return config.depthAndStencil;
			}

			inline GLuint texture() {
				return _tex;
			}

			void use(std::function<void()> exec) const;

			friend void transfer(FrameBuffer& source, FrameBuffer& destination);

		private:
			GLuint _tex;
			GLuint _fbo;
			GLuint _rbo;
			Config config;
			Status status;
			GLbitfield clearBits;
			GLint viewport[4];
		};

	}
}
#include "detail/FrameBuffer.inl"