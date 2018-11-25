#pragma once
#include <string>
#include "gl_commands.h"
#include <glm/vec2.hpp>
#include <functional>
#include <algorithm>
#include "Image.h"
#include "Noise.h"
#include "Shader.h"

#define TEXTURE(id) GL_TEXTURE0 + id

namespace ncl {
	namespace gl {

		static GLuint nextId = 0;

		using LoadData = std::function<void(void)>;

		void loadTexture(GLenum target, GLuint& buffer, GLuint id, glm::vec2 wrap, glm::vec2 minMagfilter, LoadData loadData, bool optimize = true) {
			glGenTextures(1, &buffer);
			glActiveTexture(TEXTURE(id));
			glBindTexture(target, buffer);
			loadData();
			if (optimize) {
				glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap.s);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap.t);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMagfilter.x);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, minMagfilter.y);
			}
		}

		class Texture2D {
		public:
			Texture2D(std::string path, GLuint id = nextId++, std::string name = "", GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }) : _id(id) {
				Image img(path);
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, img.width(), img.height(), 0, format, GL_UNSIGNED_BYTE, img.data()); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = img.width();
				_height = img.height();
				_name = name;
			}

			Texture2D(void* data, GLuint width, GLuint height, std::string name = "", GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_NEAREST }) : _id(id) {
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, width, height, 0, format, dataType, data); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = width;
				_height = height;
				_name = name;
			}

			Texture2D(GLuint width, GLuint height, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA) {

			}

			virtual ~Texture2D() {
				glDeleteTextures(1, &buffer);
			}

			GLuint id() { return _id; }

			GLuint bufferId() { return buffer; }

			GLuint width() { return _width; }

			GLuint height() { return _height; }

			void sendTo(Shader& shader) {
				glActiveTexture(TEXTURE(_id));
				glBindTexture(GL_TEXTURE_2D, buffer);
				shader.sendUniform1i(_name, _id);
				//glActiveTexture(TEXTURE(0));
			}

		private:
			GLuint buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;
			std::string _name;


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

			GLuint depth() { return _depth; }

		private:
			GLuint buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;
			GLuint _depth;


		};

		class NoiseTex2D : public Texture2D {
		public:
			NoiseTex2D(std::string name = "", unsigned id = nextId++, const Noise2D& noise = Perlin2D, float freq = 4.0f, float ampl = 0.5f, int width = 128, int height = 128)
				:Texture2D(noise(width, height, freq, ampl).get(), width, height, name, id, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory

			}
		private:
			GLuint * _data;
		};

		class NoiseTex3D : public Texture3D {
		public:
			NoiseTex3D(Noise3D noise = Perlin3D, float freq = 4.0f, float ampl = 0.5f, int width = 64, int height = 64, int depth = 64)
				:Texture3D(noise(freq, ampl, width, height, depth).get(), width, height, depth, nextId++, GL_RGBA8, GL_RGBA, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory

			}
		private:
			GLuint * _data;
		};

		class TextureBuffer {
		public:
			TextureBuffer(std::string name, const void* data, GLuint size, GLenum iFormat = GL_RGBA32F, GLuint bufId = 0, unsigned id = nextId++, GLenum usage = GL_STATIC_DRAW):_buffer(bufId) { // TODO fix nextId bug
				glActiveTexture(TEXTURE(id));
			
				if (glIsBuffer(_buffer) == GL_FALSE) {
					glGenBuffers(1, &_buffer);
					glBindBuffer(GL_TEXTURE_BUFFER, _buffer);
					glBufferData(GL_TEXTURE_BUFFER, size, data, usage);
					gl::objectLabel(GL_BUFFER, _buffer, "textureBuffer:buffer" + name);
				}

				glGenTextures(1, &_tbo_id);
				glBindTexture(GL_TEXTURE_BUFFER, _tbo_id);
				glTexBuffer(GL_TEXTURE_BUFFER, iFormat, _buffer);
				gl::objectLabel(GL_TEXTURE, _tbo_id, "textureBuffer:" + name);
				_id = id;
				_name = name;
			}

			virtual ~TextureBuffer() {
				glDeleteBuffers(1, &_buffer);
				glDeleteTextures(1, &_tbo_id);
			}

			GLuint tbo_id() const { return _tbo_id; }

			GLuint buffer() const { return _buffer; }

			GLuint id() const { return _id; }

			void sendTo(Shader& shader) {
				glActiveTexture(TEXTURE(_id));
				glBindTexture(GL_TEXTURE_BUFFER, _tbo_id);
				shader.sendUniform1i(_name, _id);
				glActiveTexture(TEXTURE(0));
			}

		private:
			GLuint _buffer;
			GLuint _tbo_id;
			GLuint _id;
			std::string _name;
		};

		class CheckerTexture : public Texture2D {
		public:
			CheckerTexture(unsigned id = nextId++, std::string name = "", const glm::vec4& colorA = WHITE, const glm::vec4& colorB = BLACK)
				: Texture2D(generate(colorA, colorB).get(), 256, 256, name, id, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory
			}

			static std::unique_ptr<GLubyte[]> generate(const glm::vec4& a, const glm::vec4& b) {
				// TODO cache texture
				GLubyte* data = new GLubyte[256 * 256 * 4];
				glm::vec4 color;
				for (int i = 0; i<256; i++) {
					for (int j = 0; j < 256; j++) {
						int idx = (i * 256 + j) * 4;
						color = (((i / 8) % 2) && ((j / 8) % 2)) || (!((i / 8) % 2) && !((j / 8) % 2)) ? b : a;
						data[idx] = color.r * 255;
						data[idx + 1] = color.g * 255;
						data[idx + 2] = color.b * 255;
						data[idx + 3] = color.a * 255;
					}
				}
				return std::unique_ptr<GLubyte[]>{data};
			}

		};

		class DoubleBuffer {
		public:
			DoubleBuffer(GLuint atId, GLenum format, GLuint* buffers) :DoubleBuffer(atId, format, { *buffers, *(++buffers) }) {}

			DoubleBuffer(GLuint atId, GLenum format, std::initializer_list<GLuint> buffers) {
				this->_buffers[0] = *buffers.begin();
				this->_buffers[1] = *(buffers.begin()+1);
				this->activeTexId = atId;
				glGenTextures(2, texIds);
				glBindTexture(GL_TEXTURE_BUFFER, texIds[_front]);
				glTexBuffer(GL_TEXTURE_BUFFER, format, _buffers[_front]);

				glBindTexture(GL_TEXTURE_BUFFER, texIds[_back]);
				glTexBuffer(GL_TEXTURE_BUFFER, format, _buffers[_back]);
				glBindTexture(GL_TEXTURE_BUFFER, 0);
			}

			~DoubleBuffer() {
				glDeleteTextures(2, texIds);
			}

			void activate() const {
				glActiveTexture(TEXTURE(activeTexId));
				glBindTexture(GL_TEXTURE_BUFFER, texIds[_front]);
			}

			void swapBuffers() {
				std::swap(_front, _back);
			}

			GLuint front() const {
				return _front;
			}

			GLuint back() const {
				return _back;
			}

			const GLuint* buffers() const {
				return _buffers;
			}

			GLuint buffer(int idx) const {
				return _buffers[idx];
			}

		private:
			GLuint _buffers[2];
			GLuint texIds[2];
			GLuint activeTexId;
			GLuint _front = 0, _back = 1;
		};

		class Image2D {
		public:
			Image2D(GLuint width, GLuint height, GLenum format = GL_RGBA32F, std::string name = "", GLuint id = 0, GLuint buffer = 0, GLuint img_id = 0): 
				_id(id), _buffer(buffer), _img_id(img_id), _format(format), _name(name) {
				if (glIsBuffer(_buffer) == GL_FALSE) {
					glGenTextures(1, &_buffer);
				}
				glActiveTexture(TEXTURE(_id));
				glBindTexture(GL_TEXTURE_2D, _buffer);
				glTexStorage2D(GL_TEXTURE_2D, 1, _format, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				if (_name == "") _name = std::string("image") + std::to_string(id);
				gl::objectLabel(GL_BUFFER, _buffer, "image2D:" + name);

				mode = Mode::COMPUTE;
			}

			void computeMode() {
				mode = Mode::COMPUTE;
			}

			void renderMode() {
				mode = Mode::RENDER;
			}

			GLuint buffer() {
				return _buffer;
			}

			void sendTo(Shader& shader) {
				if (mode == Mode::COMPUTE) {
					glBindImageTexture(_img_id, _buffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, _format);
					shader.sendUniform1i(_name, _img_id);
				}
				else {
					glActiveTexture(TEXTURE(_id));
					glBindTexture(GL_TEXTURE_2D, _buffer);
					shader.sendUniform1i(_name, _id);
				}
			}

		protected:
			enum Mode { COMPUTE, RENDER };

		private:
			GLuint _id;
			GLuint _buffer;
			GLuint _img_id;
			GLenum _format;
			std::string _name;
			Mode mode;
		};
	}
}