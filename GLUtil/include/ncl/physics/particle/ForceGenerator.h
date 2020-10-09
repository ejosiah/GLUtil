#pragma once

#include <glm/glm.hpp>
#include "../../gl/shaders.h"
#include "../../gl/compute.h"
#include "Particle.h"
#include <sstream>

namespace ncl {
	namespace physics {
		namespace pm {

			class ForceGenerator : public ParticleCompute {
			public:
				ForceGenerator(int id, gl::StorageBuffer<Particle>& particles, ncl::gl::Shader* shader, glm::vec3 workers = { 1, 1, 1 }) :
					ParticleCompute{ particles,  shader, workers }, _id{ id } {
				}

				ForceGenerator(int id, gl::StorageBuffer<Particle>& particles, std::string shaderSource, glm::vec3 workers = { 1, 1, 1 }) :
					ParticleCompute{ particles,  shaderSource, workers }, _id{ id } {
				}

				void preCompute() override {
					ParticleCompute::preCompute();
					ncl::gl::send("id", _id);
				}
				
				inline int id() {
					return _id;
				}

			protected:
				int _id;
			};

			class Gravity : public ForceGenerator {
			public:
				Gravity(int id, glm::vec3 value, gl::StorageBuffer<Particle>& particles, glm::vec3 workers = { 1, 1, 1 })
					:ForceGenerator{ id, particles, particle_gravity_comp_shader, workers }
					, _value{ value }
				{
				
				}

				void preCompute() override {
					ForceGenerator::preCompute();
					ncl::gl::send("gravity", _value);
				}

				//void postCompute() override {
				//	particles.read([&](pm::Particle* itr) {
				//		for (int i = 0; i < 16; i++) {
				//			auto particle = (itr + i);
				//			if (particle->active > 0) {
				//				sbr.str("");
				//				sbr.clear();
				//				sbr << "particle: " << i << "\n";
				//				sbr << "\tposition: " << particle->position << "\n";
				//				sbr << "\tvelocity: " << particle->velocity << "\n";
				//				sbr << "\tacceleration: " << particle->acceleration << "\n";
				//				sbr << "\tforces: " << particle->forceAccum << "\n";
				//				sbr << "\tdamping: " << particle->damping << "\n";
				//				sbr << "\tinvMas: " << (particle->inverseMass) << "\n";
				//				sbr << "\tgenerators: " << particle->forceGenerators << "\n";
				//				sbr << "\tactive: " << particle->active << "\n";
				//				sbr << "\tlifetime: " << particle->lifetime << "\n";
				//				auto p = *particle;
				//				logger.info(sbr.str());
				//				break;
				//			}
				//		}
				//		});
				//}

			private:
				glm::vec3 _value;
				std::stringstream sbr;
				ncl::Logger logger = ncl::Logger::get("gravity");
			};
		}
	}
}