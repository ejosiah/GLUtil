#pragma once

#include <string>
#include <functional>
#include "gl_commands.h"
#include "Shader.h"

namespace ncl {
	namespace gl {

		class TransformFeebBack {
		public:
			TransformFeebBack(std::string name = "", bool disableRaster = true, const char** varyings = nullptr, GLsizei noOfVaryings = 0, Shader* shader = nullptr, bool separate_attribs = true) {
				this->disableRaster = disableRaster;
				gl::genTransformFeedbacks(1, &tf_id); 
				gl::bindTransformFeedback(GL_TRANSFORM_FEEDBACK, tf_id);
				gl::objectLabel(GL_TRANSFORM_FEEDBACK, tf_id, "xbf:" + name);
				if (varyings != nullptr) {
					gl::transformFeedbackVaryings(shader->program(), noOfVaryings, varyings, separate_attribs ? GL_SEPARATE_ATTRIBS : GL_INTERLEAVED_ATTRIBS); 
					shader->relink();
				}
				gl::bindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
				
			}

			~TransformFeebBack() {
				gl::deleteTransformFeedbacks(1, &tf_id);
			}

			void operator()(const GLuint* buffers, const GLsizei noOfBuffers, GLenum primitiveType, std::function<void()> proc) {
				gl::bindTransformFeedback(GL_TRANSFORM_FEEDBACK, tf_id);
				for (int i = 0; i < noOfBuffers; i++) {
					gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, i, buffers[i]); 
				}
				if (disableRaster) glEnable(GL_RASTERIZER_DISCARD); 
				gl::beginTransformFeedback(primitiveType); 
				proc(); 
				gl::endTransformFeedback(); 
				if (disableRaster) glDisable(GL_RASTERIZER_DISCARD); 
				gl::bindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
			}

			void use(std::initializer_list<GLuint> buffers, const GLsizei noOfBuffers, GLenum primitiveType, std::function<void()> proc) {
				GLuint bufs[2] = { *buffers.begin(), *(buffers.begin() + 1) };
				operator()(bufs, noOfBuffers, primitiveType, proc);
			}

		private:
			GLuint tf_id;
			bool disableRaster;
			static GLuint nextId;
		};

		GLuint TransformFeebBack::nextId;

	}
}