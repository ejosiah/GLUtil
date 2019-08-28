#include "Sampler.h"
#pragma once

namespace ncl {
	namespace sampling {
		RandomSampler::RandomSampler(){
			auto dist = std::uniform_real_distribution<float>(0, 1);
			auto engine = std::default_random_engine();
			rng = std::bind(dist, engine);
		}

		RandomSampler::~RandomSampler(){}

		float RandomSampler::get1D() {
			return rng();
		}

		glm::vec2 RandomSampler::get2D() {
			return glm::vec2(rng(), rng());
		}
	}

}