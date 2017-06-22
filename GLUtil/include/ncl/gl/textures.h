#pragma once
#include <string>
#include <gl/gl_core_4_5.h>
#include <glm/vec2.hpp>
#include "Image.h"

#define TEXTURE(id) GL_TEXTURE0 + id

namespace ncl {
	namespace gl {

		static GLuint nextId = 0;

		class Texture2D {
		public:
			Texture2D(std::string path, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }): _id(id) {
				Image img(path);
				loadTexture(img.data(), img.width(), img.height(), id, iFormat, format, wrap, minMagfilter);
			}

			Texture2D(unsigned char* data, GLuint width, GLuint height, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, GLuint id = nextId++, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }) {
				loadTexture(data, width, height, id, iFormat, format, wrap, minMagfilter);
			}

			Texture2D(){}

			~Texture2D() {
				glDeleteTextures(1, &buffer);
			}

			GLuint id() { return _id;  }

			GLuint width() { return _width; }

			GLuint height() { return _height; }

		private:
			GLuint buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;

		protected:
			void loadTexture(unsigned char* data, GLuint width, GLuint height, GLuint id, GLuint iFormat, GLuint format, glm::vec2 wrap, glm::vec2 minMagfilter) {
				_width = width;
				_height = height;
				int w = width;
				int h = height;
				glGenTextures(1, &buffer);
				glActiveTexture(TEXTURE(id));
				glBindTexture(GL_TEXTURE_2D, buffer);
				glTexImage2D(GL_TEXTURE_2D, 0, iFormat, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap.s);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap.t);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagfilter.x);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagfilter.y);
			}

		};
	}
}