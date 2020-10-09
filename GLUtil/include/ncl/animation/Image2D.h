#pragma once

#include "../gl/primitives.h"
#include "../gl/textures.h"

namespace ncl {
	namespace animation {
		class Image2D : public gl::Drawable {
		public:
			Image2D() = default;

			Image2D(std::string path) {
				_texture = gl::Texture2D{ path };
				_quad = gl::ProvidedMesh{ gl::screnSpaceQuad() };	// TODO use image size
			}

			Image2D(const Image2D&) = delete;

			Image2D(Image2D&& source) noexcept {
				transfer(source, *this);
			}

			Image2D& operator=(const Image2D&) = delete;

			Image2D& operator=(Image2D&& source) noexcept {
				transfer(source, *this);
				return *this;
			}

			void draw(gl::Shader& shader) override {
				glBindTextureUnit(0, _texture.buffer());
				_quad.draw(shader);
			}

			unsigned int width() {
				return _texture.width();
			}

			unsigned int height() {
				return _texture.height();
			}

			void transfer(Image2D& source, Image2D& dest) {
				dest._quad = std::move(source._quad);
				dest._texture = std::move(source._texture);
			}

		private:
			gl::ProvidedMesh _quad;
			gl::Texture2D _texture;
		};
	}
}