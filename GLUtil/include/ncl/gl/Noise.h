#pragma once
#include <glm/gtc/noise.hpp>
#include "textures.h"
#include <functional>

namespace ncl {
	namespace gl {

		using NoiseFn = std::function<float(glm::vec2, glm::vec2)>;
		float perlin(glm::vec2 p, glm::vec2 f) { return glm::perlin(p, f); };
		float simplex(glm::vec2 p, glm::vec2 f) { return glm::simplex(p); }


		class NoiseTex2D : public Texture2D {
		public:
			NoiseTex2D(float freq = 4.0f, float ampl = 0.5f, int width = 128, int height = 128, NoiseFn noise = simplex) {
				generateTexture(freq, ampl, width, height, noise);
				loadTexture(data, width, height, nextId++, GL_RGBA8, GL_RGBA, glm::vec2{ GL_REPEAT }, glm::vec2{ GL_LINEAR });
				delete data;
			}

			GLubyte* generateTexture(float freq, float ampl, int width, int height, NoiseFn noise) {
				data = new GLubyte[width * height * 4];

				float u = 1.0f / width;
				float v = 1.0f / width;

				for (int row = 0; row < height; row++) {
					for (int col = 0; col < width; col++) {
						float x = u * col;
						float y = v * row;
						float f = freq;
						float a = ampl;

						for (int oct = 0; oct < 4; oct++) {
							glm::vec2 p{ x * f, y * f };
							float val = noise(p, glm::vec2{ f }) * a + 0.5;

							data[((row * width + col) * 4) + oct] = GLubyte(val * 255);
							f *= 2.0f;
							a *= ampl;
						}
					}
				}
				return data;
			}

		private:
			GLubyte* data;
		};
	}
}