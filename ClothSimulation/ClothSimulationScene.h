#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/compute.h"

using namespace ncl;
using namespace gl;
using namespace glm;


class ClothSimScene : public Scene {
public:

	ClothSimScene() :Scene("Cloth Simulation") {

	}

	void init() override {
		light[0].position = { 0, 5, 0, 1 };
		plane = new Plane({ {0, 1, 0} , 0 }, 100, WHITE);
	//	checkerboard = new CheckerTexture(0, "checkerbox");
		board = new CheckerBoard_gpu(1024, 1024);
		board->compute();
	}

	void display() override {

		cam.view = translate(mat4(1), { 0.0f, 0.0f, dist });
		cam.view = rotate(cam.view, radians(pitch), { 1.0f, 0.0f, 0.0f });
		cam.view = rotate(cam.view, radians(yaw), { 0.0f, 1.0f, 0.0f });

		shader("default")([&] {
			send("activeTextures[0]", true);
			send(light[0]);
			send(cam);
		//	send(checkerboard);
			board->images().front().renderMode();
			send(&board->images().front());
			shade(plane);
		});
	}

	void update(float dt) override {
	} 

	void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 1.0f, 100.f);
	}

private:
	Plane * plane;
	//Texture2D* checkerboard;
	CheckerBoard_gpu* board;
	float yaw = 0;
	float pitch = 10;
	float dist = -11;
};