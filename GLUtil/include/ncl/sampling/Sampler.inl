#include "Sampler.h"
#include "hash.h"

namespace ncl {
	namespace sampling {
		RandomSampler::RandomSampler(){
			auto dist = std::uniform_real_distribution<float>(0, 1);
			auto engine = std::default_random_engine(std::random_device{}());
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

		StratifiedSampler::StratifiedSampler(int xSamples, int ySamples, const bool jitter)
			:nSamples(xSamples * ySamples), n2DSamples(xSamples, ySamples), jitter(jitter){
			auto dist = std::uniform_real_distribution<float>(0, 1);
			auto engine = std::default_random_engine();
			rng = std::bind(dist, engine);

			auto dist1 = std::uniform_int_distribution<int>(0, xSamples);
			auto engine1 = std::default_random_engine(1000);
			rngX = std::bind(dist1, engine1);

			auto dist2 = std::uniform_int_distribution<int>(0, ySamples);
			auto engine2 = std::default_random_engine(2000);
			rngY = std::bind(dist2, engine2);
			
			current1DSample = 0;
			current2DSample = glm::vec2(0);
		}

		float StratifiedSampler::get1D() {
			current1DSample++;
			current1DSample %= nSamples;
			float delta = jitter ? rng() : 0.5f;
			return current1DSample + delta * (1.0/nSamples);
		}

		glm::vec2 StratifiedSampler::get2D() {
			float jx = jitter ? rng() : 0.5f;
			float jy = jitter ? rng() : 0.5f;

			float dx = 1.0 / n2DSamples.x;
			float dy = 1.0 / n2DSamples.y;

			auto res =  glm::vec2((rngX() + jx) * dx, (rngY() + jy) * dy);

			current2DSample.x++;

			return res;
		}
	}

}