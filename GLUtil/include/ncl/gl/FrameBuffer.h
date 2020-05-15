#pragma once

#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>

namespace ncl {
	namespace gl {

		class FrameBuffer {
		public:
			struct Attachment {
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
				GLsizei numLayers = 1;
			};
			struct Config {
				GLsizei width;
				GLsizei height;
				std::vector<Attachment> attachments;
				GLenum fboTarget = GL_FRAMEBUFFER;
				glm::vec4 clearColor = BLACK;
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

			inline GLuint texture(int index = 0) {
				return _textures.at(index);
			}

			inline int numAttachments() {
				return _textures.size();
			}

			void use(std::function<void()> exec) const;

			friend void transfer(FrameBuffer& source, FrameBuffer& destination);

			static Config defaultConfig(GLsizei width, GLsizei height);

		private:
			std::vector<GLuint> _textures;
			GLuint _fbo;
			GLuint _rbo;
			GLuint _depth_stencil_tex;
			Config config;
			Status status;
			GLbitfield clearBits;
			GLint viewport[4];
		};

	}
}
#include "detail/FrameBuffer.inl"