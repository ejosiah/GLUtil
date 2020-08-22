#pragma once

#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/Model.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleObject.h"
#include <glm/glm.hpp>

namespace gcl = ncl::gl;
namespace pm = ncl::physics::pm;

class Cannon : public gcl::SceneObject, public pm::ParticleObject {
public:
	Cannon(ncl::gl::Scene* scene, pm::ParticleRegistry& registry)
		:gcl::SceneObject(scene)
		, pm::ParticleObject{ 1, registry }{
		init();
	}

	void init() override {
		model = new gcl::Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\cannon\\Cannon.obj", true, 5);
		model->defautMaterial(false);
		diffuseMap = new gcl::Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\models\\cannon\\Cannon_UV_Difuse.tif", 0);
		specularMap = new gcl::Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\models\\cannon\\Cannon_UV_Spec.tif", 1);
		NormalMap = new gcl::Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\models\\cannon\\Cannon_UV_Bump.tif", 2);
		xform = glm::translate(glm::mat4(1), { 0, -model->aabb().min.y, 0 });
	//	xform = glm::translate(glm::mat4(1), { 0, 100, 0 });
		camera = &scene().activeCamera();

		with([&](pm::Particle* particle) {
			particle->active = 1;
			particle->forceGenerators = 1;
			particle->inverseMass = 0;
			
		});
	}

	void render(bool shadowMode = false) override {
		using namespace gcl;
		send(camera, xform);
		if (!shadowMode) {
			glBindTextureUnit(0, diffuseMap->bufferId());
			glBindTextureUnit(1, diffuseMap->bufferId());
			glBindTextureUnit(2, specularMap->bufferId());
			glBindTextureUnit(3, NormalMap->bufferId());
		}
		send("offset", _offset);
		send("isBump", true);
		shade(model);
		send("isBump", false);
		update(0);
	}

	void update(float t) override {

	}

private:
	gcl::Model* model;
	glm::mat4 xform;
	gcl::Camera* camera;
	gcl::Texture2D* diffuseMap;
	gcl::Texture2D* specularMap;
	gcl::Texture2D* NormalMap;
	std::stringstream sbr;
	ncl::Logger logger = ncl::Logger::get("gravity");
};