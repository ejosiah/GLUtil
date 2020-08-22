#pragma once


#include "../GLUtil/include/ncl/gl/Scene.h"
#include "VolumeRenderingObject.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class RayCasting : public VolumeRenderingObject {
public:
	RayCasting(Scene* scene)
		:VolumeRenderingObject{ scene, "Ray marching Volume Rendering" }
	{
		init();
	}

	void render(bool shadowMode) override {
		scene().shader("ray_marching")([&] {
			glEnable(GL_BLEND);
			glBindTextureUnit(0, texture->buffer());
			send(scene().activeCamera());
			send("stepSize", vec3(1.0f / XDIM, 1.0f / YDIM, 1.0f / ZDIM));
			send("camPos", scene().activeCamera().getPosition());
			shade(cube);
			glDisable(GL_BLEND);
		});
	}

	void init() override {
		cube =  Cube{ 1 };
	}

private:
	Cube cube;
};