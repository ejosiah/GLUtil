#pragma once

#include <stdexcept>
#include <string>
#include <sstream>
#include <gl/gl_core_4_5.h>

#ifndef CHECK_GL_ERRORS
#define CHECK_GL_ERRORS  assert(glGetError()==GL_NO_ERROR);
#endif

namespace ncl {
	namespace gl {

		inline void bindBufferBase(GLenum target, GLuint index, GLuint buffer) {
			glBindBufferBase(target, index, buffer); CHECK_GL_ERRORS
		}

		//glGenTransformFeedbacks
		//	glBindTransformFeedback
		//	glTransformFeedbackVaryings
		//	glDeleteTransformFeedbacks
		//	glBeginTransformFeedback
		//	glEndTransformFeedback

		//*************************  xbf functions ******************************************************************

		inline void genTransformFeedbacks(GLsizei n, GLuint* ids) {
			glGenTransformFeedbacks(n, ids); CHECK_GL_ERRORS
		}

		inline void transformFeedbackVaryings(GLuint program, GLsizei count, const char** varyings, GLenum bufferMode) {
			glTransformFeedbackVaryings(program, count, varyings, bufferMode); CHECK_GL_ERRORS
		}

		inline void beginTransformFeedback(GLenum primitiveMode) {
			glBeginTransformFeedback(primitiveMode); CHECK_GL_ERRORS
		}

		inline void endTransformFeedback() {
			glEndTransformFeedback(); CHECK_GL_ERRORS
		}

		inline void bindTransformFeedback(GLenum target, GLuint id) {
			glBindTransformFeedback(target, id); CHECK_GL_ERRORS
		}

		inline void deleteTransformFeedbacks(GLsizei n, GLuint* ids) {
			glDeleteTransformFeedbacks(n, ids); CHECK_GL_ERRORS
		}

		//*************************************************************************************************************

		inline void objectLabel(GLenum identifier, GLuint name, std::string label) {
			glObjectLabel(identifier, name, label.size(), label.c_str());
		}

		inline std::string getObjectLabel(GLenum identifier, GLuint name) {
			char buf[4096];
			GLsizei length;
			glGetObjectLabel(identifier, name, 4096, &length, buf);
			std::stringstream label;
			for (int i = 0; i < length; i++) label << buf[i];
			return label.str();
		}
	}
}