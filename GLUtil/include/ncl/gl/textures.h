#pragma once
#include <string>
#include <gl/gl_core_4_5.h>
#include <glm/vec2.hpp>
#include <functional>
#include "Image.h"
#include "Noise.h"

#define TEXTURE(id) GL_TEXTURE0 + id

namespace ncl {
	namespace gl {

		static GLuint nextId = 0;

		using LoadData = std::function<void(void)>;

		void loadTexture(GLenum target, GLuint& buffer, GLuint id, glm::vec2 wrap, glm::vec2 minMagfilter, LoadData loadData) {
			glGenTextures(1, &buffer);
			glActiveTexture(TEXTURE(id));
			glBindTexture(target, buffer);
			loadData();
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap.s);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap.t);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagfilter.x);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagfilter.y);
		}

		class Texture2D {
		public:
			Texture2D(std::string path, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }): _id(id) {
				Image img(path);
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, img.width(), img.height(), 0, format, GL_UNSIGNED_BYTE, img.data()); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = img.width();
				_height = img.height();
			}

			Texture2D(unsigned char* data, GLuint width, GLuint height, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }) : _id(id) {
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = width;
				_height = height;
			}

			Texture2D(){}

			virtual ~Texture2D() {
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

		
		};

		class Texture3D {
		public:
			Texture3D(unsigned char* data, GLuint width, GLuint height, GLuint depth, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_REPEAT }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR }) : _id(id) {
			/*	LoadData load = [&]() { 
					glTexImage3D(GL_TEXTURE_3D, 0, iFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data); 
					glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap.t);
				
				};
				loadTexture(GL_TEXTURE_3D, buffer, id, wrap, minMagfilter, load);
				_width = width;
				_height = height;
				_depth = depth;*/
				glGenTextures(1, &buffer);
				glActiveTexture(TEXTURE(id));
				glBindTexture(GL_TEXTURE_3D, buffer);
				glTexImage3D(GL_TEXTURE_3D, 0, iFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap.s);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap.t);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap.t);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minMagfilter.x);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, minMagfilter.y);
			}

			Texture3D() {}

			virtual ~Texture3D() {
				glDeleteTextures(1, &buffer);
			}

			GLuint id() { return _id; }

			GLuint width() { return _width; }

			GLuint height() { return _height; }

			GLuint depth() { return _depth;  }

		private:
			GLuint buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;
			GLuint _depth;


		};

		class NoiseTex2D : public Texture2D {
		public:
			NoiseTex2D(unsigned id = nextId++, const Noise2D& noise = Perlin2D, float freq = 4.0f, float ampl = 0.5f, int width = 128, int height = 128)
			:Texture2D(noise(width, height, freq, ampl).get(), width, height, id, GL_RGBA8, GL_RGBA, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }){ // TODO free data memory

			}
		private:
			GLuint* _data;
		};

		class NoiseTex3D : public Texture3D {
		public:
			NoiseTex3D(Noise3D noise = Perlin3D, float freq = 4.0f, float ampl = 0.5f, int width = 64, int height = 64, int depth = 64)
				:Texture3D(noise(freq, ampl, width, height, depth).get(), width, height, depth, nextId++, GL_RGBA8, GL_RGBA, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory

			}
		private:
			GLuint* _data;
		};

		class TextureBuffer {
		public:
			TextureBuffer(const void* data, GLenum iFormat = GL_RGBA32F, unsigned id = nextId++) {
				glActiveTexture(TEXTURE(id));
				glGenTextures(1, &tbo_id);
				glBindTexture(GL_TEXTURE_BUFFER, tbo_id);
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_TEXTURE_BUFFER, buffer);
				glBufferData(GL_TEXTURE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
				glTexBuffer(GL_TEXTURE_BUFFER, iFormat, buffer);
				glActiveTexture(GL_TEXTURE0);
			}

			virtual ~TextureBuffer() {
				glDeleteBuffers(1, &buffer);
				glDeleteTextures(1, &tbo_id);
			}

		private:
			GLuint buffer;
			GLuint tbo_id;
		};
	}
}