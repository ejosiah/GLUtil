#pragma once

#include "../../gl/BufferObject.h"
#include "Particle.h"
#include <tuple>

namespace ncl {
	namespace physics {
		namespace pm {
			class ParticleRegistry {
			public:
				ParticleRegistry() = default;

				ParticleRegistry(size_t size) 
					:_particles{ new gl::StorageBuffer<Particle>{} }
					, _offset{ 0 } 
				{
					_particles->allocate(size);
				}

				~ParticleRegistry() {
					delete _particles;
				}

				std::tuple<int, gl::StorageBuffer<Particle>*> _register(size_t count) {
					if (count < 0 || count >= _particles->count()) throw  "Index out of bounds";

					
					auto res = std::make_tuple(_offset, _particles);
					_offset += count;
					return res;
				}

				gl::StorageBuffer<Particle>& particles() {
					return *_particles;
				}

			private:
				gl::StorageBuffer<Particle>* _particles;
				int _offset;
			};
		}
	}
}