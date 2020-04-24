#pragma once
#include <gl/gl_core_4_5.h>


namespace ncl {
	namespace gl {

		enum class TexMinFilter {
			Nearest, Linear, NearestMipMapNearest, LinearMipMapNearest, NearestMipMapLinear, LinearMipMapLinear 
		};

		enum class TexMagFilter {
			Linear, Nearest
		};

		inline GLenum toGL(TexMinFilter filter) {
			switch (filter) {
			case TexMinFilter:: Nearest:
				return GL_NEAREST;
			case TexMinFilter::Linear:
				return GL_LINEAR;
			case TexMinFilter::NearestMipMapNearest:
				return GL_NEAREST_MIPMAP_NEAREST;
			case TexMinFilter::LinearMipMapNearest:
				return GL_LINEAR_MIPMAP_NEAREST;
			case TexMinFilter::NearestMipMapLinear:
				return GL_NEAREST_MIPMAP_LINEAR;
			case TexMinFilter::LinearMipMapLinear:
				return GL_LINEAR_MIPMAP_LINEAR;
			}
		}

		inline GLenum toGL(TexMagFilter filter) {
			switch (filter) {
			case TexMagFilter::Linear:
				return GL_LINEAR;
			case TexMagFilter::Nearest:
				return GL_NEAREST;
			}
		}
	}
}