#pragma once

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <functional>
#include "Shape.h"
#include "Shader.h"
#include "textures.h"

namespace ncl {
	namespace gl {
		class VaryingCapture {
		public:
			using Proc = std::function<void()>;

			virtual GLsizei size() = 0;

			virtual std::string bufferName() const = 0;

			virtual std::tuple<GLsizei, char**, Shader*> varyings() {
				return std::make_tuple(0, nullptr, nullptr);
			};
			
			virtual void initializeVaryings() {
				GLsizei noOfVaryings;
				char** varyingNames;
				Shader* shader;

				std::tie(noOfVaryings, varyingNames, shader) = varyings();

				_buffer = new TextureBuffer(bufferName(), nullptr, size(), GL_RGBA32F, 0, 1, GL_DYNAMIC_READ);
				xbf = new TransformFeebBack("xbf:shape" + bufferName(), false, (const char**)varyingNames, noOfVaryings, shader, false);
			}

			virtual void captureVarying(Proc proc, bool disableRaster = true) {
				if (disableRaster) glEnable(GL_RASTERIZER_DISCARD);
				GLuint bufId = buffer();
				(*xbf)(&bufId, 1, GL_TRIANGLES, proc);
				if (disableRaster) glDisable(GL_RASTERIZER_DISCARD);
			}


			GLuint buffer() {
				return _buffer->buffer();
			}

		private:
			TextureBuffer* _buffer;
			TransformFeebBack* xbf;
		};
	}
}