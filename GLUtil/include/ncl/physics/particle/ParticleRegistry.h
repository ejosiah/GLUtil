#pragma once

#include "../../gl/StorageBufferObj.h"
#include "Particle.h"
#include <tuple>

namespace ncl {
	namespace physics {
		namespace pm {
			class ParticleRegistry {
			public:
				ParticleRegistry() = default;

				ParticleRegistry(size_t size) 
					:_particles{ new gl::StorageBufferObj<Particle>{ size, 0 } }
					, _offset{ 0 } {

				}

				~ParticleRegistry() {
					delete _particles;
				}

				std::tuple<int, gl::StorageBufferObj<Particle>*> _register(size_t count) {
					if (count < 0 || count >= _particles->count()) throw  "Index out of bounds";

					
					auto res = std::make_tuple(_offset, _particles);
					_offset += count;
					return res;
				}

				gl::StorageBufferObj<Particle>& particles() {
					return *_particles;
				}

			private:
				gl::StorageBufferObj<Particle>* _particles;
				int _offset;
			};
		}
	}
}