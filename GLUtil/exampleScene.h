#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/models.h"
#include "include/ncl/gl/UICore.h"
#include "include/ncl/gl/layouts.h"
#include "include/ncl/gl/button.h"
#include "include/ncl/gl/frame.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;
using namespace ui;

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
		addShaderFromFile("shaders\\quad.vert");
		addShaderFromFile("shaders\\quad.tes");
		addShaderFromFile("shaders\\identity.frag");
		_requireMouse = false;
	}

	virtual void init() override {
		delete _motionEventHandler;
		_motionEventHandler = new _3DMotionEventLogger(cam);
		cam.view = glm::lookAt(vec3(1.0f, 1.5f, 1.25f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		font = Font::Arial(20, 0, BLACK);
	//	sphere = new Sphere(0.1, 50, 50);
	//	teapot = new Teapot(8);
		cube = new Cube;
		model = new Model("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\models\\bigship1.obj", true);
		lightModel.twoSided = true;
		lightModel.colorMaterial = true;
		m.ambient = m.diffuse = vec4(1, 0, 0, 1);
		m.specular = vec4(1);
		_shader.sendUniformMaterial("material[1]", m);
		glClearColor(0.8, 0.8, 0.8, 1);
		cam.model = mat4(1);
		


		UI::init();
		container.setLayout(new FlowLayout());

		Button* button0 = new Button("Click me 0");
		Button* button1 = new Button("Click me 1");
		Button* button2 = new Button("Click me 2");

		container.setPosition(10, 10);
		container.add(button0);
		container.add(button1);
		container.add(button2);
		container.setDimensions(700, 100);
		container.show();

		_mouseClickListners.push_back([&](Mouse& mouse) {
			UI::handleMouseClick(mouse);
		});

		_mouseMoveListner.push_back([&](Mouse& mouse) {
			UI::handleMouseMove(mouse);
		});

	}

	virtual void resized() override {
		cam.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
		UI::resize(_width, _height);
	}

	virtual void display() override {
	//	_shader.sendUniform1i("grids", 16);
	//	cam.model = mat4(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
	//	_shader.send(cam);
		//teapot->draw(_shader);
		_shader.send(cam);
		cube->draw(_shader);

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
	HiResCube* hiResCube;
	Material m;
	ui::Frame container;
	Font* font;
	
};