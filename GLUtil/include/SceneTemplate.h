#pragma once
#include <glm/glm.hpp>
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/Scene.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class SceneTemplate : public Scene {


public:
	using Scene::Scene;

	virtual void init() override {

	}

	virtual void onResize(int w, int h) {
	}

	virtual void display() {

	}

	virtual void update(float elapsedTime) {

	}
};