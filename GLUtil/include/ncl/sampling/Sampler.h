#pragma once

#include <glm/vec2.hpp>
#include <functional>
#include <random>

namespace ncl {
	namespace sampling {


		class Sampler {
		public:
			virtual ~Sampler() = default;
			virtual float get1D() = 0;
			virtual glm::vec2 get2D() = 0;
		};

		class RandomSampler : public Sampler {
		public:
			RandomSampler();
			virtual ~RandomSampler();
			virtual float get1D() override;
			virtual glm::vec2 get2D() override;

		private:
			std::function<float()> rng;
		};

		class GLSLSampler : public Sampler {
		public:
			GLSLSampler();
			virtual ~GLSLSampler() = default;
			virtual float get1D() override;
			virtual glm::vec2 get2D() override;
		private:
			glm::vec2 xySeed;
			float seed;
			const float Max = 1e10;
		};

		class StratifiedSampler : public Sampler {
		public:
			StratifiedSampler(int xSamples = 100, int ySamples = 10, const bool jitter = false);
			virtual ~StratifiedSampler() = default;
			virtual float get1D() override;
			virtual glm::vec2 get2D() override;
		private:
			int nSamples;
			glm::vec2 n2DSamples;
			int current1DSample;
			glm::vec2 current2DSample;
			const bool jitter;
			std::function<float()> rng;
			std::function<int()> rngX;
			std::function<int()> rngY;
		};
	}
}

#include "Sampler.inl"