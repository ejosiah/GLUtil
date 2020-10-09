#pragma once

#include <tuple>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include "../GLUtil/include/ncl/physics/particle/Particle.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleRegistry.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleObject.h"
#include "../GLUtil/include/ncl/gl/Model.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/util.h"

namespace gcl = ncl::gl;
namespace pm = ncl::physics::pm;

enum class ShotType {
	Pistol, Artillery, Fireball, Lazer
};

class CannonBall : public gcl::SceneObject, public pm::ParticleObject {
public:
	CannonBall(gcl::Scene* scene, pm::ParticleRegistry& registry, size_t count = 16)
		:gcl::SceneObject{ scene }
		, pm::ParticleObject{count, registry}
	{
		init();
	}

	void init() override {
		using namespace gcl;
		using namespace pm;
		
		direction = normalize(glm::vec3{ -3.61, 2.93, 0 } -startPos);
		auto rng = ncl::rngReal(0, 10);


		with([&](Particle* itr) {
			for (int i = 0; i < _count; i++) {
				auto particle = (itr + i);
				//		particle->position = glm::vec3(rng(), rng(), rng());
				particle->position = startPos + direction * 1.0f;
				particle->velocity = glm::vec3(0);
				particle->acceleration = glm::vec3(0);
				//particle->forceAccum = glm::vec3(0);
				//particle->damping = 0;
				//particle->inverseMass = 0;
				particle->forceGenerators = 0;
				particle->active = false;
				particle->lifetime = 0;
				//	particle->stuff.w = 0;
			}
			});
		

		std::vector<glm::mat4> xforms;
		for (int i = 0; i < _count; i++) {
			xforms.push_back(glm::mat4(1));
			//xforms.push_back(glm::translate(glm::mat4(1), glm::vec3(rng(), rng(), rng())));
		}

		model = gcl::Sphere{ 0.2, 50, 50, gcl::BLACK, (unsigned)_count, xforms };
		model.defautMaterial(false);
	//	model = gcl::Sphere{ 0.2, 50, 50, gcl::BLACK };
		diffuse = new Texture2D{ path + "MetalSpottyDiscoloration001_COL_1K_SPECULAR.jpg" };
		specular = new Texture2D{ path + "MetalSpottyDiscoloration001_REFL_1K_SPECULAR.jpg" };
		normal = new Texture2D{ path + "MetalSpottyDiscoloration001_NRM_1K_SPECULAR.jpg" };
	}


	void render(bool shaodwMode = false) override {
		using namespace gcl;
		using namespace pm;

		scene().shader("particle_lfp")([&] {	
			_particles->sendToGPU();
			glBindTextureUnit(0, diffuse->buffer());
			glBindTextureUnit(1, diffuse->buffer());
			glBindTextureUnit(2, specular->buffer());
			glBindTextureUnit(3, normal->buffer());
			send("isBump", false);
			send("offset", _offset);
			send(scene().activeCamera());
			shade(model);

		});
		update(0);
	}


	void fire() {
		with([&](pm::Particle* itr) {
			for (int i = 0; i < _count; i++) {
				auto particle = (itr + i);
				if (!particle->active) {

					switch (type) {
					case ShotType::Artillery:
						particle->inverseMass = 0.005; // 200kg
						particle->velocity = direction * 50.f;	// 50m/s
						particle->acceleration = { 0.0f, -20.0f, 0.0f };
						particle->damping = 0.99f;
						break;
					case ShotType::Pistol:
						particle->inverseMass = 0.5;	// 2kg
						particle->velocity = direction * 35.f;	// 35m/s
						particle->acceleration = { 0.0f, -1.0f, 0.0f };
						particle->damping = 0.99f;
						break;
					case ShotType::Fireball:
						particle->inverseMass = 1;	// kg
						particle->velocity = direction * 5.f;	// 5m/s
						particle->acceleration = { 0.0f, 0.6f, 0.0f };
						particle->damping = 0.9f;
						break;
					case ShotType::Lazer:
						particle->inverseMass = 10;	// 0.1kg
						particle->velocity = direction * 100.0f;	// 100m/s
						particle->acceleration = { 0.0f, 0.0f, 0.0f };
						particle->damping = 0.99f;
						break;
					}

					particle->active = true;
					particle->position = startPos;
					particle->forceAccum = glm::vec3(0);
					break;
				}	
			}
			});
	}

	std::string shotType() {
		switch (type) {
		case ShotType::Artillery:
			return "Artillery";
		case ShotType::Pistol:
			return "Pistol";
		case ShotType::Fireball:
			return "Fireball";
		case ShotType::Lazer:
			return "Lazer";
		}
	}

	void nextShotType() {
		id++;
		id %= 4;
		type = types[id];
	}

	void update(float t) override {
		with([&](pm::Particle* itr) {
			//for (int i = 0; i < _count; i++) {
			//	auto particle = (itr + i);
			//	if (particle->active) {
			//		sbr.str("");
			//		sbr.clear();
			//		sbr << "particle: " << "\n";
			//		sbr << "\tposition: " << particle->position << "\n";
			//		sbr << "\tvelocity: " << particle->velocity << "\n";
			//		sbr << "\tacceleration: " << particle->acceleration << "\n";
			//		sbr << "\tforces: " << particle->forceAccum << "\n";
			//		sbr << "\tdamping: " << particle->damping << "\n";
			//		sbr << "\tinvMas: " << (particle->inverseMass) << "\n";
			//		sbr << "\tgenerators: " << particle->forceGenerators << "\n";
			//		sbr << "\tactive: " << particle->active << "\n";
			//		sbr << "\tlifetime: " << particle->lifetime << "\n";
			//		auto p = *particle;
			//		logger.info(sbr.str());
			//	}
			//}
		});
	}

private:
	gcl::Sphere model;
	glm::vec3 startPos{ 0.504, 1.6, 0 };
	glm::vec3 direction;
	gcl::Texture2D* diffuse;
	gcl::Texture2D* specular;
	gcl::Texture2D* normal;
	std::string path = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\MetalSpottyDiscoloration001\\\SPECULAR\\1K\\";
	ShotType type = ShotType::Artillery;
	ShotType types[4] = { ShotType::Artillery, ShotType::Pistol, ShotType::Fireball, ShotType::Lazer };
	int id = 0;
	std::stringstream sbr;
	ncl::Logger logger = ncl::Logger::get("CannonBall");
};