#pragma once

#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <initializer_list>

namespace ncl {
	namespace gl {

		class FrameBuffer {
		public:
			struct Attachment {
				GLenum texTarget = GL_TEXTURE_2D;
				GLint texLevel = 0;
				GLint internalFmt = GL_RGBA;
				GLint fmt = GL_RGBA;
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
				bool mipMap = false;
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
				bool internalTexture = true;
			};

			enum class Status{ Complete, UnComplete};

			FrameBuffer() = default;

			FrameBuffer(Config, std::function<void()> extraTexConfig = [] {});

			FrameBuffer(const FrameBuffer&) = delete;

			FrameBuffer(FrameBuffer&&) noexcept;

			~FrameBuffer();

			FrameBuffer& operator=(const FrameBuffer&) = delete;

			FrameBuffer& operator=(FrameBuffer&&) noexcept;

			void attachTextureFor(GLuint layer, GLuint level = 0) const;

			void attachTextureFor(GLuint layer, GLuint level, std::initializer_list<int> attachments) const;

			inline bool rboBuf() const {
				return config.depthAndStencil;
			}

			inline GLuint texture(int index = 0) {
				return _textures.at(index);
			}

			inline void setTexture(Texture2D* texture, int index = 0) {
				setTexture(texture->buffer(), 0);
			}

			inline void setTexture(Texture3D* texture, int index = 0) {
				setTexture(texture->buffer(), 0);
			}
			inline void setTexture(Texture2D& texture, int index = 0) {
				setTexture(texture.buffer(), 0);
			}

			inline void setTexture(Texture3D& texture, int index = 0) {
				setTexture(texture.buffer(), 0);
			}

			inline void setTexture(GLuint texture, int index = 0) {
				assert(_textures.size() > index);
				glBindFramebuffer(config.fboTarget, _fbo);
				_textures[index] = texture;
				glFramebufferTexture(config.fboTarget, config.attachments[index].attachment, texture, 0);
				glBindFramebuffer(config.fboTarget, 0);
			}

			inline int numAttachments() {
				return _textures.size();
			}

			inline GLuint fbo() {
				return _fbo;
			}

			inline GLuint rbo() {
				return _rbo;
			}

			void use(std::function<void()> exec, GLuint layer = 0) const;

			void use(Texture2D* texture, std::function<void()> exec, GLuint layer = 0);

			void use(Texture3D* texture, std::function<void()> exec, GLuint layer = 0);

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