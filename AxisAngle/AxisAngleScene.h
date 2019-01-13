#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include <glm/glm.hpp>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class AxisAngleScene : public Scene {
public:
	AxisAngleScene():Scene("Axis Angle"){
	}

	void init() override {
		crossHairs = new CrossHair;
		cam.view = lookAt({ 0, 0, 8 }, vec3(0), { 0, 1, 0 });
		setBackGroundColor(CYAN);

		auto& camController = getActiveCameraController();
		camController.setModelHeight(5);
		camController.updateMode(Camera::Mode::ORBIT);
		camController.init();

		vec3 A{ 4, 0, 0 };
		vec3 B{ 0, 3, 0 };

		float s = glm::sqrt(2) / 2.0f;

		axisIn = { s, s, 0 };
		vIn = A - B;

		axis = new Vector(axisIn, vec3(0), 1.0, MAGENTA);
		a = new Vector(A);
		b = new Vector(B);
		v = new Vector(vIn, vec3(0), 1.0, RED);
	}

	void display() override {
		shader("old_default")([&] {
			send(light[0]);
			send(cam);
		//	shade(crossHairs);
			shade(axis);
			shade(v);
			shade(a);
			shade(b);
		});
	}

	void resized() override {
		cam.projection = perspective(radians(60.0f), aspectRatio, 0.3f, 1000.0f);
	}

private:
	CrossHair* crossHairs;
	Vector* axis;
	Vector* v;
	Vector* a;
	Vector* b;
	float angle;
	vec3 axisIn;
	vec3 vIn;

};