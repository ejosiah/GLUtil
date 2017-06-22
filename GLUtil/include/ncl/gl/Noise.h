#pragma once
#include <glm/gtc/noise.hpp>
#include "textures.h"
#include <functional>
#include <memory>

namespace ncl {
	namespace gl {

		using NoiseFn2D = std::function<float(glm::vec2, glm::vec2)>;
		using NoiseFn3D = std::function<float(glm::vec3, glm::vec3)>;

		float perlin2d(glm::vec2 p, glm::vec2 f) { return glm::perlin(p, f); }
		float perlin3d(glm::vec3 p, glm::vec3 f) { return glm::perlin(p, f); }
		float simplex2d(glm::vec2 p, glm::vec2 f) { return glm::simplex(p); }
		float simplex3d(glm::vec3 p, glm::vec3 f) { return glm::simplex(p); }

		class Noise2D {
		public:
			Noise2D(NoiseFn2D noise) :noise(noise) {}

			std::unique_ptr<GLubyte> operator()(float freq, float ampl, int width, int height) const {
				GLubyte* data = new GLubyte[width * height * 4];

				float u = 1.0f / width;
				float v = 1.0f / height;

				for (int row = 0; row < height; row++) {
					for (int col = 0; col < width; col++) {
						glm::vec2 plane{ u * col, v * row };
						float f = freq;
						float a = ampl;

						for (int oct = 0; oct < 4; oct++) {
							glm::vec2 p = plane * f;
							float val = noise(p, glm::vec2{ f }) * a + 0.5;

							data[((row * width + col) * 4) + oct] = GLubyte(val * 255);
							f *= 2.0f;
							a *= ampl;
						}
					}
				}
				return std::unique_ptr<GLubyte>{data};
			}
		private:
			NoiseFn2D noise;
		};

		class Noise3D {
		public:
			Noise3D(NoiseFn3D noise) :noise(noise) {}

			std::unique_ptr<GLubyte> operator()(float freq, float ampl, int width, int height, int depth) const {
				GLubyte* data = new GLubyte[width * height * depth * 4];

				float u = 1.0f / width;
				float v = 1.0f / height;
				float w = 1.0f / depth;

				for (int row = 0; row < height; row++) {
					for (int col = 0; col < width; col++) {
						for (int dep = 0; dep < depth; dep++) {
							glm::vec3 cube{ u * col, v * row, w * dep };
							float f = freq;
							float a = ampl;

							for (int oct = 0; oct < 4; oct++) {
								glm::vec3 p = cube * f;
								float val = noise(p, glm::vec3{ f }) * a + 0.5;

								data[((row * width + col) * depth + dep) + oct] = GLubyte(val * 255);
								f *= 2.0f;
								a *= ampl;
							}
						}
					}
				}
				return std::unique_ptr<GLubyte>{data};
			}
		private:
			NoiseFn3D noise;
		};

		const Noise2D Perlin2D{ perlin2d };
		const Noise2D Simplex2D{ simplex2d };
		const Noise3D Perlin3D{ perlin3d };
		const Noise3D Simplex3D{ simplex3d };
	}
}