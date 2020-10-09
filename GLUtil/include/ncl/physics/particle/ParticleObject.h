#pragma once


#include <functional>
#include <sstream>
#include <glm/glm.hpp>
#include <tuple>
#include "Particle.h"
#include "ParticleRegistry.h"
#include "../../gl/BufferObject.h"

namespace ncl {
	namespace physics {
		namespace pm {
			class ParticleObject {
			public:
				ParticleObject(size_t count, ParticleRegistry& registry)
					: _count{count}
					, _offset{0}
					,_particles{ nullptr } {

					std::tie(_offset, _particles) = registry._register(count);
				}


				gl::StorageBuffer<Particle>& particles() {
					return *_particles;
				}

				inline int count() {
					return _count;
				}

				inline int offset() {
					return _offset;
				}

				inline void with(std::function<void(Particle*)> proc) {
					_particles->read([&](Particle* itr) {
						proc(itr + _offset);
					});
				}

			protected:
				size_t _count;
				int _offset;
				gl::StorageBuffer<Particle>* _particles;
			};
		}
	}
}