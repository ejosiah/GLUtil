#pragma once
#include <glm/gtc/noise.hpp>
#include "textures.h"
#include <functional>
#include <memory>
#include "logger.h"
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>


namespace ncl {
	namespace gl {

		using NoiseFn2D = std::function<float(glm::vec2, glm::vec2)>;
		using NoiseFn3D = std::function<float(glm::vec3, glm::vec3)>;

		float perlin2d(glm::vec2 p, glm::vec2 f) { return glm::perlin(p, f); }
		float perlin3d(glm::vec3 p, glm::vec3 f) { return glm::perlin(p, f); }
		float simplex2d(glm::vec2 p, glm::vec2 f) { return glm::simplex(p); }
		float simplex3d(glm::vec3 p, glm::vec3 f) { return glm::simplex(p); }
		float noOPNoise(glm::vec2 p, glm::vec2 f) { throw "No noise available"; }

		class Noise2D {
		public:
			Noise2D(NoiseFn2D noise) :noise(noise) {}

			virtual std::unique_ptr<GLubyte[]> operator()(int width, int height, float freq = 4.0f, float ampl = 0.5f) const {
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
				return std::unique_ptr<GLubyte[]>{data};
			}
		private:
			NoiseFn2D noise;
		};

		class Noise3D {
		public:
			Noise3D(NoiseFn3D noise) :noise(noise) {}

			std::unique_ptr<GLubyte[]> operator()(float freq, float ampl, int width, int height, int depth) const {
				GLubyte* data = new GLubyte[width * height * depth * 4];

				int f, i, j, k, inc;
				int startFrequency = 4;
				int numOctaves = 4;
				glm::vec3 ni;
				double inci, incj, inck;
				int frequency = startFrequency;
				GLubyte *ptr;
				unsigned int noise3DTexSize = width;
				double amp = 0.5;

				for (f = 0, inc = 0; f < numOctaves;
				++f, frequency *= 2, ++inc, amp *= 0.5)
				{
					ptr = data;
					ni[0] = ni[1] = ni[2] = 0;
					inci = 1.0 / (noise3DTexSize / frequency);
					for (i = 0; i < noise3DTexSize; ++i, ni[0] += inci)
					{
						incj = 1.0 / (noise3DTexSize / frequency);
						for (j = 0; j < noise3DTexSize; ++j, ni[1] += incj)
						{
							inck = 1.0 / (noise3DTexSize / frequency);
							for (k = 0; k < noise3DTexSize;
							++k, ni[2] += inck, ptr += 4)
							{
								*(ptr + inc) = (GLubyte)(((noise(ni * float(frequency), glm::vec3(frequency)) * amp) + 0.5)
									* 255);
							}
						}
					}
				}

				return std::unique_ptr<GLubyte[]>{data};
			}
		private:
			NoiseFn3D noise;
		};

		using DistanceFn = std::function<float(float, float, float, float)>;
		using LayoutFn = std::function<void(GLubyte*, float*, int, int)>;

		DistanceFn Euclidean = [=](float x, float x1, float y, float y1) { return hypot(x - x1, y - y1); };
		DistanceFn Manhattan = [=](float x, float x1, float y, float y1) { return abs(x - x1) + abs(y - y1); };

		LayoutFn defaultLayout = [=](GLubyte* data, float* dist, int c, int offset) {
			int n = dist[0];

			data[offset] = n;
			data[offset + 1] = n;
			data[offset + 2] = n;
			data[offset + 3] = c;
		};

		LayoutFn invertLayout = [=](GLubyte* data, float* dist, int c, int offset) {
			int n = dist[0];

			data[offset] = ~n;
			data[offset + 1] = ~n;
			data[offset + 2] = ~n;
			data[offset + 3] = ~c;
		};

		class WorleyNoise2D : public Noise2D {
		public:
			WorleyNoise2D(DistanceFn distance = Euclidean, LayoutFn layout = defaultLayout, int m = 0) :
				m(m), distance(distance), layout(layout), seed(ncl::nextSeed()), Noise2D(noOPNoise) {

			}

			virtual std::unique_ptr<GLubyte[]> operator()(int width, int height, float freq = 4.0, float ampl = 0.5) const override {
				using namespace ncl;
				int n = 100;
				auto rngX = rngReal(0, width - 1);
				auto rngY = rngReal(0, height - 1);
				std::vector<float> seedsX(n, 0);
				std::vector<float> seedsY(n, 0);

				std::generate_n(seedsX.begin(), n, [&]() { return rngX(); });
				std::generate_n(seedsY.begin(), n, [&]() { return rngY(); });

				float maxDist = 0.0;
				for (int ky = 0; ky < height; ky++) {
					for (int kx = 0; kx < width; kx++) {
						int i = -1;
						std::vector<float> dist(n, 0);
						std::generate_n(dist.begin(), n, [&] { return distance(seedsX[++i], kx, seedsY[i], ky); });
						std::sort(dist.begin(), dist.end());
						maxDist = dist[m] > maxDist ? dist[m] : maxDist;
					}
				}

				GLubyte* data = new GLubyte[width * height * 4];

				for (int ky = 0; ky < height; ky++) {
					for (int kx = 0; kx < width; kx++) {
						int i = -1;
						std::vector<float> dist(n, 0);
						std::generate_n(dist.begin(), n, [&] { return distance(seedsX[++i], kx, seedsY[i], ky); });
						std::sort(dist.begin(), dist.end());
						int c = int(round(255 * dist[m] / maxDist));
						int offset = ((ky * width + kx) * 4);

						layout(data, &dist[0], c, offset);

					}
				}
				return std::unique_ptr<GLubyte[]>{data};
			}

		private:
			int m;
			DistanceFn distance;
			LayoutFn layout;
			unsigned int seed;
		};

		const Noise2D Perlin2D{ perlin2d };
		const Noise2D Simplex2D{ simplex2d };
		const Noise3D Perlin3D{ perlin3d };
		const Noise3D Simplex3D{ simplex3d };
		const WorleyNoise2D worleyNoise;
	}
}