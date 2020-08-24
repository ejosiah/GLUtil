#pragma once
#include <string>
#include "gl_commands.h"
#include <glm/vec2.hpp>
#include <variant>
#include <functional>
#include <algorithm>
#include "Image.h"
#include "Noise.h"
#include "Shader.h"
#include "common.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace ncl {
	namespace gl {

		struct Data {
			void* contents;
			GLsizei width;
			GLsizei height;
		};

		using Content = std::variant<std::string, Data>;
		struct Config {
			std::vector<Content> contents;
			std::string name = "";
			GLuint id = 0;
			GLuint internalFmt = GL_RGBA8;
			GLuint format = GL_RGBA;
			GLenum wrapS = GL_REPEAT;
			GLenum wrapT = GL_REPEAT;
			GLenum wrapR = GL_REPEAT;
			GLenum minFilter = GL_NEAREST;
			GLenum magFilter = GL_NEAREST;
			GLint border = 0;
			GLsizei numLayers = 1;
			bool mipMap = false;
			GLenum type = GL_UNSIGNED_BYTE;
		};

		struct TextureVisitor {

			inline TextureVisitor(GLenum dataType = GL_UNSIGNED_BYTE)
				:dataType{ dataType } {}

			inline Data operator()(std::string path) {
				auto image = Image(path, glTypeToILType(dataType));	// TODO data type
				return {
					image.data(),
					image.height(),
					image.width()
				};
			}

			inline Data operator()(Data data) {
				return data;
			}

			GLenum dataType;
		};

		struct Texture {
			GLint unit;
			GLuint buffer;
		};

		static GLuint nextId = 0;

		using LoadData = std::function<void(void)>;

		void loadTexture(GLenum target, GLuint& buffer, GLuint id, glm::vec2 wrap, glm::vec2 minMagfilter, LoadData loadData, bool optimize = true) {
			glGenTextures(1, &buffer);
		//	glActiveTexture(TEXTURE(id));
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
			Texture2D(std::string path, GLuint id = 0, std::string name = "", GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec2 wrap = glm::vec2{ GL_REPEAT }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR })
				: _id(id) {
				Image img(path);
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, img.width(), img.height(), 0, format, GL_UNSIGNED_BYTE, img.data()); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = img.width();
				_height = img.height();
				_name = name;
			}

			Texture2D(void* data, GLuint width, GLuint height, std::string name = "", GLuint id = 0, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR }) 
				: _id(id) {
				LoadData load = [&]() { glTexImage2D(GL_TEXTURE_2D, 0, iFormat, width, height, 0, format, dataType, data); };
				loadTexture(GL_TEXTURE_2D, buffer, id, wrap, minMagfilter, load);
				_width = width;
				_height = height;
				_name = name;

			//	delete[] data;	TODO fix this, should not be shared
			}

			Texture2D(Config c) :config{ c } {

			}

			Texture2D(GLuint texture, GLuint unit = 0) {
				if (glIsTexture(texture) == GL_FALSE) {
					throw std::to_string(texture) + "is not a valid texture object";
				}
				this->buffer = texture;
				this->_id = unit;
				this->_width = 0;
				this->_height = 0;
				this->_name = "";
				this->deleteBuffer = false;
			}

			Texture2D(const Texture2D& source) {
				this->buffer = source.buffer;
				this->_id = source._id;
				this->_width = source._width;
				this->_height = source._height;
				this->_name = source._name;
				this->deleteBuffer = false;
			}

			Texture2D(Texture2D&& source) noexcept {
				transfer(source, *this);
			}

			virtual ~Texture2D() {
				if (deleteBuffer && glIsTexture(buffer) == GL_TRUE) {
					glDeleteTextures(1, &buffer);
				}
			}

			Texture2D& operator=(const Texture2D& source) noexcept {
				this->buffer = source.buffer;
				this->_id = source._id;
				this->_width = source._width;
				this->_height = source._height;
				this->_name = source._name;
				this->deleteBuffer = false;
				return *this;
			}

			Texture2D& operator=(Texture2D&& source) noexcept {
				transfer(source, *this);
				return *this;
			}

			friend inline void transfer(Texture2D& source, Texture2D& dest) {
				dest._id = source._id;
				dest.buffer = source.buffer;
				dest._width = source._width;
				dest._height = source._height;
				dest._name = source._name;
				dest.deleteBuffer = source.deleteBuffer;

				source.buffer = 0;
			}

			GLuint id() { return _id; }

			GLuint unit() { return _id; }

			GLuint bufferId() { return buffer; }

			GLuint width() { return _width; }

			GLuint height() { return _height; }

			void sendTo(Shader& shader) {
				//glActiveTexture(TEXTURE(_id));
				//glBindTexture(GL_TEXTURE_2D, buffer);
				glBindTextureUnit(_id, buffer);
			//	if(!_name.empty()) shader.sendUniform1i(_name, _id);
				//glActiveTexture(TEXTURE(0));
			}

		private:
			GLuint buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;
			std::string _name;
			Config config;
			bool deleteBuffer = true;


		};

		class Texture3D {
		public:
			Texture3D(void* data, GLuint width, GLuint height, GLuint depth, GLuint id = nextId++, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, glm::vec3 wrap = glm::vec3{ GL_REPEAT }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR }, GLenum type = GL_UNSIGNED_BYTE, std::function<void()> extraOptions = [] {}) 
				: _id{ id }
				, _width{ width }
				, _height{ height }
				, _depth{ depth }
			{
				/*	LoadData load = [&]() {
				glTexImage3D(GL_TEXTURE_3D, 0, iFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap.t);

				};
				loadTexture(GL_TEXTURE_3D, buffer, id, wrap, minMagfilter, load);
				_width = width;
				_height = height;
				_depth = depth;*/
				glGenTextures(1, &_buffer);
				glActiveTexture(TEXTURE(id));
				glBindTexture(GL_TEXTURE_3D, _buffer);
				glTexImage3D(GL_TEXTURE_3D, 0, iFormat, width, height, depth, 0, format, type, data);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap.s);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap.t);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap.r);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minMagfilter.x);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, minMagfilter.y);
				extraOptions();

			//	delete[] data;
			}

			virtual ~Texture3D() {
				glDeleteTextures(1, &_buffer);
			}

			inline GLuint id() { return _id; }
	
			inline GLuint buffer() { return _buffer;  }

			inline GLuint width() { return _width; }

			inline GLuint height() { return _height; }

			inline GLuint depth() { return _depth; }

		private:
			GLuint _buffer;
			GLuint _id;
			GLuint _width;
			GLuint _height;
			GLuint _depth;


		};

		class NoiseTex2D : public Texture2D {
		public:
			NoiseTex2D(std::string name = "", GLuint id = 0, const Noise2D& noise = Perlin2D, float freq = 4.0f, float ampl = 0.5f, int width = 128, int height = 128)
				:Texture2D(noise(width, height, freq, ampl).get(), width, height, name, id, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory

			}
		private:
			GLuint * _data;
		};

		class NoiseTex3D : public Texture3D {
		public:
			NoiseTex3D(Noise3D noise = Perlin3D, float freq = 4.0f, float ampl = 0.5f, int width = 64, int height = 64, int depth = 64)
				:Texture3D(noise(freq, ampl, width, height, depth).get(), width, height, depth, nextId++, GL_RGBA8, GL_RGBA, glm::vec3{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory

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

			TextureBuffer(const TextureBuffer&&) = delete;

			TextureBuffer(TextureBuffer&& source) {
				transfer(source, *this);
			}

			TextureBuffer& operator=(const TextureBuffer&&) = delete;

			TextureBuffer& operator=(TextureBuffer&& source) {
				transfer(source, *this);
				return *this;
			}

			virtual ~TextureBuffer() {
				if (glIsBuffer(_buffer) == GL_TRUE) {
					glDeleteBuffers(1, &_buffer);
				}
				if (glIsTexture(_tbo_id)) {
					glDeleteTextures(1, &_tbo_id);
				}
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

			friend inline void transfer(TextureBuffer& source, TextureBuffer& dest) {
				dest._buffer = source._buffer;
				dest._tbo_id = source._tbo_id;
				dest._id = source._id;
				dest._name = source._name;

				source._buffer = 0;
				source._tbo_id = 0;
				source._name = "";
			}

		private:
			GLuint _buffer;
			GLuint _tbo_id;
			GLuint _id;
			std::string _name;
		};

		class CheckerTexture : public Texture2D {
		public:
			CheckerTexture(GLuint id = 0, std::string name = "", const glm::vec4& colorA = WHITE, const glm::vec4& colorB = BLACK)
				: Texture2D(generate(colorA, colorB).get(), 256, 256, name, id, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR }) { // TODO free data memory
			}

			CheckerTexture(const CheckerTexture&) = delete;

			CheckerTexture(CheckerTexture&& source) noexcept 
				:Texture2D(static_cast<Texture2D&&>(source)) {}

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

			CheckerTexture& operator=(CheckerTexture&& source) noexcept {
				Texture2D::operator=(static_cast<Texture2D&&>(source));
				return *this;
			}

			CheckerTexture& operator=(const CheckerTexture&) = delete;

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
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Todo pass in gl_tex_params
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
					glBindImageTexture(_img_id, _buffer, 0, GL_FALSE, 0, GL_READ_WRITE, _format);
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
		inline Texture2D* load_hdr_texture(std::string path, GLuint textureUnit, std::string name = "") {

			stbi_set_flip_vertically_on_load(true);
			int width, height, nrComponents;
			void* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
			if (data) {
				return new Texture2D{
					data,
					(GLuint)width,
					(GLuint)height,
					name,
					textureUnit,
					GL_RGB16F,
					GL_RGB,
					GL_FLOAT,
					{GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
					{GL_LINEAR, GL_LINEAR}
				};
				stbi_image_free(data);
			}
			else {
				return nullptr;
			}
		}

		//class Texture2DArray {
		//public:
		//	Texture2DArray() = default;

		//	Texture2DArray(Config config, GLuint unit = 0);

		//	Texture2DArray(const Texture2DArray&) = delete;

		//	Texture2DArray(Texture2DArray&&);

		//	Texture2DArray& operator=(const Texture2DArray&) = delete;

		//	Texture2DArray& operator=(Texture2DArray&&);

		//	GLuint buffer();

		//	GLuint unit();

		//	inline Texture view() {
		//		return { _unit, _buffer };
		//	}

		//	friend void transfer(Texture2DArray& source, Texture2DArray& dest);

		//private:
		//	GLuint _buffer;
		//	GLuint _unit;
		//	Config config;
		//};
	}
}

//#include "detail/textures.inl"