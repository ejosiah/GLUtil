#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class PbrScene : public Scene {
public:
	PbrScene() :Scene("PBR Scene", 1280, 960) {

	}

	void init() override {
		auto& cam = activeCamera();
		cam.lookAt({ 0, 0, 5 }, vec3(0), { 0, 1, 0 });
		
		lightModel.colorMaterial = true;
		lightModel.twoSided = true;

		sphere = new Sphere(2.0f);
		objectToWorld = translate(mat4(1), { 0, 2, 0 });
		worldToObject = inverse(objectToWorld);

		glDisable(GL_CULL_FACE);
	}

	void display() override {
		light[0].on;
		light[0].position = vec4(activeCamera().getPosition(), 1);

		shader("default")([&](Shader& s) {
			send("gammaCorrect", false);
			send(light[0]);
			send(activeCamera(), objectToWorld);
			send(lightModel);
			shade(sphere);
		});
	}

	void resized() override {
		activeCamera().perspective(60.0f, float(_width) / _height, 0.01, 1000.0f);
	}

private:
	Sphere* sphere = nullptr;
	mat4 objectToWorld;
	mat4 worldToObject;
	LightModel lightModel;
};