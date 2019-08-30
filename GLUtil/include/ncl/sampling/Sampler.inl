#include "Sampler.h"
#include "hash.h"

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

		GLSLSampler::GLSLSampler(){
			seed = 0;
			xySeed = glm::vec2(0);
		}

		float GLSLSampler::get1D() {
			float x = hash11(seed);
			seed++;
			return x;
		}

		glm::vec2 GLSLSampler::get2D() {
			glm::vec2 x = hash22(xySeed);
			xySeed.x += 1;
			xySeed.y += 1;
			return x;
		}
	}

}