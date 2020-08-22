#pragma once


#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/Model.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include <glm/glm.hpp>

namespace gcl = ncl::gl;

class Floor : public gcl::SceneObject {
public:
	Floor(gcl::Scene* scene, glm::vec2 dim = glm::vec2{ 2000 })
		: gcl::SceneObject(scene)
		, _dim{ dim }
	{
		init();
	}

	void init() override {
		using namespace gcl;
		plane = new Plane(4, 4, _dim.x, _dim.y, _dim.x/40);
		checkerboard = new CheckerTexture(0, "", WHITE, GRAY);
		plane->material().ambientMat = checkerboard->bufferId();
		plane->material().diffuseMat = checkerboard->bufferId();
		plane->material().specularMat = checkerboard->bufferId();
	}

	void render(bool shadowMode = false) override {
		using namespace gcl;
		if (shadowMode) {
			plane->defautMaterial(false);
		}
		else {
			plane->defautMaterial(true);
		}
		
		send("useNormalMap", false);
		send(&scene().activeCamera());
		send("useUV1", true);
		shade(plane);
		send("offset", -1);
		send("useUV1", false);
	}

private:
	gcl::Plane* plane;
	glm::vec2 _dim;
	gcl::CheckerTexture* checkerboard;
};