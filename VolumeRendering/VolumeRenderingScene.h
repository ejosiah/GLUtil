#pragma once

#include <fstream>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "_3dTextureSlicing.h"
#include "RayCasting.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class VolumeRenderingScene : public Scene {
public:
	VolumeRenderingScene() :Scene("Volume Rendering Techniques") {
		camInfoOn = true;
		addShaderFromFile("slicer", "shaders/common.geom");
		_modelHeight = 0.1;
	}

	void init() override {

	//	addShader("ray_marching", GL_VERTEX_SHADER, identity_vert_shader);
	//	addShader("ray_marching", _uncompiled_sources.find("ray_marching.frag")->second);

		initDefaultCamera();
		activeCamera().lookAt({ -0.7, 0.06, 0.7 }, vec3(0), { 0, 1, 0 });
		volumenRenders.push_back(new _3dTextureSlicing{ this });
		volumenRenders.push_back(new RayCasting{ this });
		itr = volumenRenders.begin();
		current = *itr;
		setBackGroundColor({ 0.5, 0.5, 1, 1 });
		glDisable(GL_CULL_FACE);
	}

	void display() override {
		current->render();
	//	renderText(10, 10, current->name());
	}

	void update(float dt) override {
		current->update(dt);
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case ' ':
				++itr;
				itr = itr != volumenRenders.end() ? itr : volumenRenders.begin();
				current = *itr;

			}
		}
		current->processInput(key);
	}


private:
	VolumeRenderingObject* current;
	_3dTextureSlicing* _3dSlicing;
	std::vector<VolumeRenderingObject*> volumenRenders;
	std::vector< VolumeRenderingObject*>::iterator itr;
	RayCasting* rayCasting;
	
};