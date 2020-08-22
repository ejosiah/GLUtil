#pragma once

#include <tuple>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"
#include "../GLUtil/include/ncl/physics/particle/Particle.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleRegistry.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleObject.h"
#include "../GLUtil/include/ncl/physics/particle/SpringForceGenerator.h"
#include "../GLUtil/include/ncl/gl/Model.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/util.h"

namespace gcl = ncl::gl;
namespace pm = ncl::physics::pm;

class Rope : public gcl::SceneObject, public pm::ParticleObject {
public:
	Rope(gcl::Scene* scene, pm::ParticleRegistry& registry, size_t count = 20)
		:gcl::SceneObject{ scene }
		, pm::ParticleObject{ count, registry }
	{
		init();
	}

	void init() override {
		std::vector<glm::mat4> xforms;
		int l = 2;
		for (int i = 0; i < _count; i++) {
			pm::SpringData spring;
			spring.l = l;
			spring.k = 50;
			spring.otherId = i + 
		}
	}
};