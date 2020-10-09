#pragma once

#include <glm/glm.hpp>
#include "../../gl/common.h"
#include "../../gl/compute.h"
#include "../../gl/shader_binding.h"
#include "../../gl/Shader.h"

namespace ncl {
	namespace physics {
		namespace pm {
#pragma pack(push,1)
			struct Particle {
				glm::vec3 position = glm::vec3(0);
				float damping = 1.0f;

				glm::vec3 prevPosition = glm::vec3(0);
				float lifetime = 0.0f;

				glm::vec3 velocity = glm::vec3(0);
				float inverseMass = 0;

				glm::vec3 acceleration = glm::vec3(0);
				int forceGenerators = 0;

				glm::vec3 forceAccum = glm::vec3(0);
				int active = 0;

				//int varlet = 0;
				//int iteration = 0;
				//glm::vec2 padding0;

			};
#pragma pack(pop)


			class ParticleCompute : public gl::Compute {
			public:
				ParticleCompute(gl::StorageBuffer<Particle>& particles, std::string shaderSource, glm::vec3 workers) :
					Compute{ workers, {}, shaderSource }, particles{ particles } {
				}

				ParticleCompute(gl::StorageBuffer<Particle>& particles, gl::Shader* shader, glm::vec3 workers) :
					Compute{ workers, {}, shader }, particles{ particles } {
				}

				void preCompute() override {
					particles.sendToGPU();
					auto count = particles.count();
					gl::send("numParticles", particles.count());
				}

			protected:
				gl::StorageBuffer<Particle>& particles;
			};

		}
	}
}