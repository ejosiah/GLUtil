#pragma once

#include <functional>
#include <gl/gl_core_4_5.h>
#include "Shader.h"

namespace ncl {
	namespace gl {

		class TransformFeebBack {
		public:
			TransformFeebBack(bool disableRaster = true, const char** varyings = nullptr, GLsizei noOfVaryings = 0, Shader* shader = nullptr, bool separate_attribs = true) {
				this->disableRaster = disableRaster;
				glGenTransformFeedbacks(1, &tf_id); 
				if (varyings != nullptr) {
					glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tf_id); 
					glTransformFeedbackVaryings(shader->program(), noOfVaryings, varyings, separate_attribs ? GL_SEPARATE_ATTRIBS : GL_INTERLEAVED_ATTRIBS); 
					shader->relink();
					glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
				}

			}

			~TransformFeebBack() {
				glDeleteTransformFeedbacks(1, &tf_id);
			}

			void operator()(const GLuint* buffers, const GLsizei noOfBuffers, GLenum primitiveType, std::function<void()> proc) {
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tf_id);
				for (int i = 0; i < noOfBuffers; i++) {
					glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, i, buffers[i]); 
				}
				if (disableRaster) glEnable(GL_RASTERIZER_DISCARD); 
				glBeginTransformFeedback(primitiveType); 
				proc(); 
				glEndTransformFeedback(); 
				if (disableRaster) glDisable(GL_RASTERIZER_DISCARD); 
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
			}

		private:
			GLuint tf_id;
			bool disableRaster;
			static GLuint nextId;
		};

		GLuint TransformFeebBack::nextId;

	}
}