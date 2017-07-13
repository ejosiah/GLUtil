#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/models.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class _3DMotionEventLogger : public _3DMotionEventHandler {
public:
	_3DMotionEventLogger(GlmCam& cam) :cam{ cam } {
	}

	Logger logger = Logger::get("3D Motion Event");

	virtual void onMotion(const _3DMotionEvent& event) override {
		string msg;
		msg = "translation[" + to_string(event.translation.x) + ", " + to_string(event.translation.y) + ", " + to_string(event.translation.z) +  "]";
		logger.info(msg);
		msg = "rotation[" + to_string(event.rotation.x) + ", " + to_string(event.rotation.y) + ", " + to_string(event.rotation.z) + "]";
		logger.info(msg);
		auto trans = event.translation * vec3 {1, -1, 1};
		cam.view = translate(cam.view, event.translation * vec3(1, 1, -1) * 0.0011f);
		cam.view = rotate(cam.view, radians(event.rotation.z * 0.001f), vec3(0, 0, 1));
		cam.view = rotate(cam.view, radians(event.rotation.y * 0.001f), vec3(0, 1, 0));
		cam.view = rotate(cam.view, radians(event.rotation.x * 0.001f), vec3(1, 0, 0));
 	};
	virtual void onNoMotion() override {};

private:
	GlmCam& cam;
};


class ExampleScene : public Scene {


public:
	ExampleScene(Options ops) :Scene("Example Scene", ops) {
		//addShaderFromFile("shaders\\quad.vert");
		//addShaderFromFile("shaders\\quad.tes");
		//addShaderFromFile("shaders\\identity.frag");
		_requireMouse = false;
	}

	virtual void init() override {
		delete _motionEventHandler;
		_motionEventHandler = new _3DMotionEventLogger(cam);
		cam.view = glm::lookAt(vec3(1.0f, 1.5f, 1.25f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		font = Font::Arial(20, 0, BLACK);
		sphere = new Sphere(0.5, 50, 50);
	//	teapot = new Teapot(8);
		cube = new Cube;
		model = new Model("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\models\\bigship1.obj", true);
		lightModel.twoSided = true;
		lightModel.colorMaterial = true;
		m.ambient = m.diffuse = vec4(1, 0, 0, 1);
		m.specular = vec4(1);
		shader("default").sendUniformMaterial("material[1]", m);
		glClearColor(0.8, 0.8, 0.8, 1);
		cam.model = mat4(1);
		


	}

	virtual void resized() override {
		cam.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
	}

	virtual void display() override {
	//	_shader.sendUniform1i("grids", 16);
	//	cam.model = mat4(1);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
	//	_shader.send(cam);
		//teapot->draw(_shader);
		shader("default").sendComputed(cam);
		sphere->draw(shader("default"));

//		cam.model = translate(mat4(1), { 0, 1, 0 });


	//	_shader.send(cam);
	//	sphere->draw(_shader);
	//	UI::render();
	//	font->render("The Quick Brown Fox Jumps over the Lazy Dog", 0, _height - 20);
	}

	virtual void update(float elapsedTime) override {

	}

private:
	Cube* cube;
	Model* model;
	Sphere* sphere;
	Teapot* teapot;
	ProvidedMesh* controlPoints;
	ProvidedMesh* controlLines;
	Material m;
	Font* font;
	
};