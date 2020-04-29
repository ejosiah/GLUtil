#pragma once

#include "../../../glm/vec_util.h"

namespace ncl {
	namespace gl {
		FrameBuffer::FrameBuffer(Config c) :config{ c } {
			glGenFramebuffers(1, &_fbo);
			glBindFramebuffer(c.fboTarget, _fbo);

			glGenTextures(1, &_tex);
			glBindTexture(c.texTarget, _tex);
			glTexImage2D(c.texTarget, c.texLevel, c.internalFmt, c.width, c.height, c.border, c.fmt, c.type, NULL);	// TODO chose based on target
			glTexParameteri(c.texTarget, GL_TEXTURE_MIN_FILTER, c.minfilter);
			glTexParameteri(c.texTarget, GL_TEXTURE_MAG_FILTER, c.magFilter);
			//glBindTexture(c.texTarget, 0);

			glFramebufferTexture2D(c.fboTarget, c.attachment, c.texTarget, _tex, c.texLevel);

			if (c.depthAndStencil) {
				glGenRenderbuffers(1, &_rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, c.width, c.height);
			//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}
			glFramebufferRenderbuffer(c.fboTarget, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

			if (glCheckFramebufferStatus(c.fboTarget) == GL_FRAMEBUFFER_COMPLETE) {
				status = Status::Complete;
			}
			else {
				status = Status::UnComplete;
			}
			glBindFramebuffer(c.fboTarget, 0);
			clearBits = GL_COLOR_BUFFER_BIT;
			clearBits = c.depthAndStencil ? clearBits | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : clearBits;
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
			auto [r, g, b, a] = toTuple(config.clearColor);

			glBindFramebuffer(config.fboTarget, _fbo);
			glClearColor(r, g, b, a);
			glClear(clearBits);
			exec();
			glBindFramebuffer(config.fboTarget, 0);
		}
	}
}