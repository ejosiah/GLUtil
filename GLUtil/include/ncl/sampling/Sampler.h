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
			GLSLSampler(int seed = std::random_device()());
			virtual ~GLSLSampler();
			virtual float get1D() override;
			virtual glm::vec2 get2D() override;
		};
	}
}

#include "Sampler.inl"