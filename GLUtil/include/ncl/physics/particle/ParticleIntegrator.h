#pragma once

#include <glm/glm.hpp>
#include "../../gl/shaders.h"
#include "../../gl/compute.h"
#include "Particle.h"
#include <sstream>

namespace ncl {
	namespace physics {
		namespace pm {
			
			class ParticleIntegrator : public ParticleCompute {
			public:
				ParticleIntegrator(gl::StorageBufferObj<Particle>& particles, glm::vec3 workers = { 1, 1, 1 }) :
					ParticleCompute{ particles,  particle_integrator_comp_shader, workers } {
				}



				void postCompute() override {
					//particles.read([&](pm::Particle* itr) {
					//	for (int i = 0; i < 16; i++) {
					//		auto particle = (itr + i);
					//		if (particle->active > 0) {
					//			sbr.str("");
					//			sbr.clear();
					//			sbr << "particle: " << i << "\n";
					//			sbr << "\tposition: " << particle->position << "\n";
					//			sbr << "\tvelocity: " << particle->velocity << "\n";
					//			sbr << "\tacceleration: " << particle->acceleration << "\n";
					//			sbr << "\tforces: " << particle->forceAccum << "\n";
					//			sbr << "\tdamping: " << particle->damping << "\n";
					//			sbr << "\tinvMas: " << (particle->inverseMass) << "\n";
					//			sbr << "\tgenerators: " << particle->forceGenerators << "\n";
					//			sbr << "\tactive: " << particle->active << "\n";
					//			sbr << "\tlifetime: " << particle->lifetime << "\n";
					//			auto p = *particle;
					//			logger.info(sbr.str());
					//			break;
					//		}
					//	}
					//});
				}

			private:
				ncl::Logger logger = ncl::Logger::get("integrator");
				std::stringstream sbr;
			};
		}
	}
}