#pragma once

#include "Particle.h"
#include "ForceGenerator.h"
#include "../../gl/StorageBufferObj.h"
#include "../../gl/common.h"

namespace ncl {
	namespace physics {
		namespace pm {
#pragma pack(push, 1)
			struct SpringData {
				int otherId;
				float k;
				float l;
				padding_4 padding0;
			};
#pragma pack(pop)

			class SpringForceGenerator : public ForceGenerator {
			public:
				SpringForceGenerator(int id, gl::StorageBufferObj<Particle>& particles, glm::vec3 workers = { 1, 1, 1 })
					:ForceGenerator{ id, particles, particle_spring_comp_shader, workers } 
				{
					springData = gl::StorageBufferObj<SpringData>{ particles.count(), 1 };
				}

				void add(int particleId, SpringData data) {
					springData.read([&](SpringData* itr) {
						auto spring = (itr + particleId);
						spring->otherId = data.otherId;
						spring->k = data.k;
						spring->l = data.l;
					});
					particles.read([&](Particle* itr) {
						auto particle = (itr + particleId);
						particle->forceGenerators |= _id;
					});
				}

			private:
				gl::StorageBufferObj<SpringData> springData;
			};
		}
	}
}