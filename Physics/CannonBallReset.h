#pragma once

#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/physics/particle/Particle.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include <glm/glm.hpp>

namespace pm = ncl::physics::pm;
namespace gl = ncl::gl;

class CannonBallReset : public pm::ParticleCompute {
public:
	CannonBallReset(gl::StorageBuffer<pm::Particle>& particles, gl::Scene& scene, glm::vec3 workers = { 1, 1, 1 })
		:pm::ParticleCompute{ particles,  &scene.shader("ballistics_rest"), workers } {
	}
};