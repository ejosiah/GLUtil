#pragma once

#include "../../../glm/vec_util.h"

namespace ncl {
	namespace gl {

		static inline bool isColorAttachment(GLenum attachment) {
			for (int i = 0; i < 32; i++) {
				if (attachment == GL_COLOR_ATTACHMENT0 + i) {
					return true;
				}
			}
			return false;
		}

		static std::vector<GLenum> getColorAttachments(const std::vector<FrameBuffer::Attachment>& attachments) {
			auto colorAttachments = std::vector<GLenum>{};
			for (auto& a : attachments) {
				if (isColorAttachment(a.attachment)) {
					colorAttachments.push_back(a.attachment);
				}
			}

			return colorAttachments;
		}

		static void initTexImage(const FrameBuffer::Config c, const FrameBuffer::Attachment& a) {
			switch (a.texTarget) {
			case GL_TEXTURE_2D:
				glTexImage2D(a.texTarget, a.texLevel, a.internalFmt, c.width, c.height, a.border, a.fmt, a.type, nullptr);
				break;
			case GL_TEXTURE_CUBE_MAP:
				for (int i = 0; i < 6; i++) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, a.internalFmt, c.width, c.height, 0, a.fmt, a.type, nullptr);
				}
				break;
			case GL_TEXTURE_2D_ARRAY:
				glTexImage3D(a.texTarget, 0, a.internalFmt, c.width, c.height, a.numLayers, a.border, a.fmt, a.type, nullptr);
				break;
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				glTexImage3D(a.texTarget, 0, a.internalFmt, c.width, c.height, a.numLayers * 6, a.border, a.fmt, a.type, nullptr);
				break;
			default:
				throw std::to_string(a.attachment) + "Not yet implemented";
			}
		}

		static inline bool containsCubeMapForColorAttachment(std::vector<FrameBuffer::Attachment>& attachments) {
			return std::any_of(attachments.begin(), attachments.end(), [](FrameBuffer::Attachment a) {
				return (a.texTarget == GL_TEXTURE_CUBE_MAP || a.texTarget == GL_TEXTURE_CUBE_MAP_ARRAY) && isColorAttachment(a.attachment);
			});
		}

		FrameBuffer::FrameBuffer(Config c, std::function<void()> extraTexConfig) :config{ c } {
			glGenFramebuffers(1, &_fbo);
			glBindFramebuffer(c.fboTarget, _fbo);

			_textures.resize(c.attachments.size());
			glGenTextures(c.attachments.size(), &_textures[0]);


			for (int i = 0; i < c.attachments.size(); i++) {
				auto a = c.attachments[i];
				auto& _tex = _textures[i];
				glBindTexture(a.texTarget, _tex);
				initTexImage(c, a);


				glTexParameteri(a.texTarget, GL_TEXTURE_MIN_FILTER, a.minfilter);
				glTexParameteri(a.texTarget, GL_TEXTURE_MAG_FILTER, a.magFilter);
				glTexParameteri(a.texTarget, GL_TEXTURE_WRAP_S,		a.wrap_s);
				glTexParameteri(a.texTarget, GL_TEXTURE_WRAP_T,		a.wrap_t);
				extraTexConfig();

				if (a.texTarget == GL_TEXTURE_CUBE_MAP || a.texTarget == GL_TEXTURE_3D) {
					glTexParameteri(a.texTarget, GL_TEXTURE_WRAP_T, a.wrap_r);
				}

				if (!config.write) glDrawBuffer(GL_NONE);
				if (!config.read) glReadBuffer(GL_NONE);

				if (a.wrap_s == GL_CLAMP_TO_BORDER) {
					glTexParameterfv(a.texTarget, GL_TEXTURE_BORDER_COLOR, a.borderColor);
				}
				if (a.mipMap) {
					glGenerateMipmap(a.texTarget);
				}
				glFramebufferTexture(c.fboTarget, a.attachment, _tex, 0);
			}

			if (c.depthAndStencil) {
				if (containsCubeMapForColorAttachment(c.attachments)) {
					glGenTextures(1, &_depth_stencil_tex);
					glBindTexture(GL_TEXTURE_CUBE_MAP, _depth_stencil_tex);
					for (int i = 0; i < 6; i++) {
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, c.width, c.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
					}
					glFramebufferTexture(c.fboTarget, GL_DEPTH_ATTACHMENT, _depth_stencil_tex, 0);
				}
				else {
					glGenRenderbuffers(1, &_rbo);
					glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, c.width, c.height);
					//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
					glFramebufferRenderbuffer(c.fboTarget, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
				}
			}

			if (c.attachments.size() > 1 && (c.read || c.write)) {
				auto attachments = getColorAttachments(c.attachments);
				glDrawBuffers(attachments.size(), &attachments[0]);
			}

			auto checkStatus = glCheckFramebufferStatus(c.fboTarget);
			if (checkStatus == GL_FRAMEBUFFER_COMPLETE) {
				status = Status::Complete;
			}
			else {
				status = Status::UnComplete;
			}

			assert(status == Status::Complete, "frambuffer incomplete");

			glBindFramebuffer(c.fboTarget, 0);
			clearBits = c.read || c.write ? GL_COLOR_BUFFER_BIT : 0;
			clearBits = c.depthTest ? clearBits | GL_DEPTH_BUFFER_BIT : clearBits;
			clearBits = c.stencilTest ? clearBits | GL_STENCIL_BUFFER_BIT : clearBits;
		}

		FrameBuffer::FrameBuffer(FrameBuffer&& source) noexcept {
			transfer(source, *this);
		}

		FrameBuffer& FrameBuffer::operator=(FrameBuffer&& source) noexcept {
			transfer(source, *this);
			return *this;
		}		inline void transfer(FrameBuffer& source, FrameBuffer& destination) {
			destination._fbo = source._fbo;
			destination._textures = std::move(source._textures);
			destination._rbo = source._rbo;
			destination._depth_stencil_tex = source._depth_stencil_tex;
			destination.config = source.config;
			destination.status = source.status;
			destination.clearBits = source.clearBits;

			source._fbo = 0;
			source._rbo = 0;
			source._depth_stencil_tex = 0;
			source.status = FrameBuffer::Status::UnComplete;
			source.config = {};

		}

		FrameBuffer::~FrameBuffer() {
			for (auto _tex : _textures) {
				if (config.deleteTexture && glIsTexture(_tex) == GL_TRUE) {
					glDeleteTextures(1, &_tex);
				}
			}
			if (glIsRenderbuffer(_rbo) == GL_TRUE) {
				glDeleteRenderbuffers(1, &_rbo);
			}
			if (glIsFramebuffer(_fbo) == GL_TRUE) {
				glDeleteFramebuffers(1, &_fbo);
			}
		}

		void FrameBuffer::use(std::function<void()> exec, GLuint layer) const {
			glGetIntegerv(GL_VIEWPORT, const_cast<GLint*>(viewport));

			glBindFramebuffer(config.fboTarget, _fbo);



			auto [r, g, b, a] = toTuple(config.clearColor); 
			glClearColor(r, g, b, a);
			glClear(clearBits);
			glViewport(viewport[0], viewport[1], config.width, config.height);
			exec();
			glBindFramebuffer(config.fboTarget, 0);
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		}

		void FrameBuffer::attachTextureFor(GLuint layer, GLuint level) const{
			for (int i = 0; i < config.attachments.size(); i++) {
				auto& a = config.attachments[i];
				auto _tex = _textures[i];
				if (a.texTarget == GL_TEXTURE_2D_ARRAY || a.texTarget == GL_TEXTURE_CUBE_MAP_ARRAY) {	// Todo check for all array textures
					glFramebufferTextureLayer(config.fboTarget, a.attachment, _tex, level, layer);
				}
				else {
					glFramebufferTexture(config.fboTarget, a.attachment, _tex, level);
				}
			}
			//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[0], 0, layer);
			//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[1], 0, layer);
			//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _textures[2], 0, layer);
		}

		void FrameBuffer::attachTextureFor(GLuint layer, GLuint level, std::initializer_list<int> attachments) const {
			for (auto i : attachments) {
				auto& a = config.attachments[i];
				auto _tex = _textures[i];
				if (a.texTarget == GL_TEXTURE_2D_ARRAY || a.texTarget == GL_TEXTURE_CUBE_MAP_ARRAY) {	// Todo check for all array textures
					glFramebufferTextureLayer(config.fboTarget, a.attachment, _tex, level, layer);
				}
				else {
					glFramebufferTexture2D(config.fboTarget, a.attachment, a.texTarget, _tex, level);
				}
			}
		}

		FrameBuffer::Config FrameBuffer::defaultConfig(GLsizei width, GLsizei height) {
			return Config{ width, height, {Attachment{}} };
		}
	}
}