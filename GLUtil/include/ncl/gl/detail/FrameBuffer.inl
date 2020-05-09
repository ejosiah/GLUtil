#pragma once

#include "../../../glm/vec_util.h"

namespace ncl {
	namespace gl {
		FrameBuffer::FrameBuffer(Config c, std::function<void()> extraTexConfig) :config{ c } {
			glGenFramebuffers(1, &_fbo);
			glBindFramebuffer(c.fboTarget, _fbo);

			glGenTextures(1, &_tex);
			glBindTexture(c.texTarget, _tex);

			if (c.texTarget == GL_TEXTURE_CUBE_MAP) {
				for (int i = 0; i < 6; i++) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, c.internalFmt, c.width, c.height, 0, c.fmt, c.type, nullptr);
				}
			}
			else {
				glTexImage2D(c.texTarget, c.texLevel, c.internalFmt, c.width, c.height, c.border, c.fmt, c.type, nullptr);
			}
			
			
			glTexParameteri(c.texTarget, GL_TEXTURE_MIN_FILTER, c.minfilter);
			glTexParameteri(c.texTarget, GL_TEXTURE_MAG_FILTER, c.magFilter);
			glTexParameteri(c.texTarget, GL_TEXTURE_WRAP_S, c.wrap_s);
			glTexParameteri(c.texTarget, GL_TEXTURE_WRAP_T, c.wrap_t);
			extraTexConfig();

			if (c.texTarget == GL_TEXTURE_CUBE_MAP || c.texTarget == GL_TEXTURE_3D) {
				glTexParameteri(c.texTarget, GL_TEXTURE_WRAP_T, c.wrap_r);
			}

			if (!config.write) glDrawBuffer(GL_NONE);
			if (!config.read) glReadBuffer(GL_NONE);

			if (c.wrap_s == c.wrap_t == GL_CLAMP_TO_BORDER) {
				glTexParameterfv(c.texTarget, GL_TEXTURE_BORDER_COLOR, c.borderColor);
			}

			//glBindTexture(c.texTarget, 0);
			//if (c.texTarget == GL_TEXTURE_2D) {
			//	glFramebufferTexture2D(c.fboTarget, c.attachment, c.texTarget, _tex, c.texLevel);
			//}
//			else if (c.texTarget == GL_TEXTURE_CUBE_MAP) {
				glFramebufferTexture(c.fboTarget, c.attachment,  _tex, 0);
//			}

			if (c.depthAndStencil) {
				glGenRenderbuffers(1, &_rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, c.width, c.height);
			//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
				glFramebufferRenderbuffer(c.fboTarget, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
			}

			if (glCheckFramebufferStatus(c.fboTarget) == GL_FRAMEBUFFER_COMPLETE) {
				status = Status::Complete;
			}
			else {
				status = Status::UnComplete;
			}
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
		}

		inline void transfer(FrameBuffer& source, FrameBuffer& destination) {
			destination._fbo = source._fbo;
			destination._tex = source._tex;
			destination._rbo = source._rbo;
			destination.config = source.config;
			destination.status = source.status;
			destination.clearBits = source.clearBits;

			source._fbo = 0;
			source._tex = 0;
			source._rbo = 0;
			source.status = FrameBuffer::Status::UnComplete;
			source.config = {};

		}

		FrameBuffer::~FrameBuffer() {
			if (config.deleteTexture && glIsTexture(_tex) == GL_TRUE) {
				glDeleteTextures(1, &_tex);
			}
			if (glIsRenderbuffer(_rbo) == GL_TRUE) {
				glDeleteRenderbuffers(1, &_rbo);
			}
			if (glIsFramebuffer(_fbo) == GL_TRUE) {
				glDeleteFramebuffers(1, &_fbo);
			}
		}

		void FrameBuffer::use(std::function<void()> exec) const {
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
	}
}