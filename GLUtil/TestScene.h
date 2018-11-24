#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/ncl/gl/Scene.h"
#include "include/ncl/gl/Shader.h"
#include "include/ncl/gl/Cube.h"
#include "include/ncl/gl/Model.h"
#include "include/ncl/gl/Plane.h"
#include "include/ncl/gl/logger.h"
#include "include/ncl/gl/Image.h"
#include "include/ncl/gl/util.h"
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/shaders.h"
#include "include/ncl/gl/Camera.h"
#include "include/ncl/gl/UserCameraController.h"
#include "include/glm/vec_util.h"

using namespace std;
using namespace ncl;
using namespace ncl::gl;
using namespace glm;

const string USERNAME = getEnv("username");

class SpaceShip {
public:
	SpaceShip(const CameraController& cameraController, Scene& scene)
		:cameraController(cameraController), scene(scene){}

	void init() {
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//shader.loadFromFile(GL_VERTEX_SHADER, "shaders/identity.vert");
		shader.loadFromstring(GL_VERTEX_SHADER, pass_through_vert_shader);
		shader.loadFromstring(GL_FRAGMENT_SHADER, pass_through_frag_shader);
		shader.createAndLinkProgram();

		phongShader.loadFromstring(GL_VERTEX_SHADER, per_fragment_lighing_vert_shader);
		phongShader.loadFromstring(GL_GEOMETRY_SHADER, wireframe_geom_shader);
		phongShader.loadFromstring(GL_FRAGMENT_SHADER, per_fragment_lighing_frag_shader);

		phongShader.createAndLinkProgram();
		string path = "C:\\Users\\" + USERNAME + "\\OneDrive\\media\\models\\bigship1.obj";
	//	string path = "C:\\Users\\" + USERNAME + "\\OneDrive\\media\\models\\Game_model\\Game_model.obj";
	//	string path = "C:\\Users\\Josiah\\Documents\\Visual Studio 2015\\Projects\\LitScene\\media\\blocks.obj";
		model = new Model(path, true);
		model->forEachMaterial([](Material& m) { m.shininess =  128.0f; });
	}

	void draw() {
		const Camera& camera = cameraController.getCamera();
		if (camera.getMode() == Camera::ORBIT) {
			//Shader& phongShader = teapot->shader;
			phongShader([&](Shader& shader) {
				LightModel lm;
				lm.localViewer = true;
				//lm.useObjectSpace = true;
				LightSource light = calculateLight(camera);
				phongShader.send("celShading", true);
				phongShader.sendUniform3fv("globalAmbience", 1, &glm::vec4(0.2)[0]);
				phongShader.send("localViewer", true);
				phongShader.sendUniform1f("line.width", 0.1);
				phongShader.sendUniform4f("line.color", 1, 1, 1, 1);
				phongShader.send("wireframe", false);
				phongShader.sendUniformMatrix4fv("viewport", 1, GL_FALSE, value_ptr(getViewport()));
				phongShader.sendUniformLight(light);
				phongShader.sendComputed(camera, cameraController.modelTrans());
				phongShader.send(lm);
				model->draw(shader);
				
			});
		}
		else {
			shader([&](Shader& shader) {
				shader.send(camera);
				model->draw(shader);
			});
		}

		scene.shader("flat")([&]() {
			send(camera);
			shade(model->bound);
		});

	}
	
	void drawWith(const Camera& camera) {
		if (cameraController.getCamera().getMode() == Camera::ORBIT) {
		//	Shader& phongShader = teapot->shader;

			phongShader([&](Shader& shader) {
				LightSource light = calculateLight(cameraController.getCamera());
				phongShader.sendUniform1i("grids", 16);
				phongShader.sendUniform1i("celShading", false);
				phongShader.sendUniform3fv("globalAmbience", 1, &vec4(0.2)[0]);
				phongShader.sendUniform1ui("localViewer", true);
				phongShader.sendUniform1ui("wireframe", false);
				shader.sendUniform1ui("eyesAtCamera", true);
				phongShader.sendUniformLight("light0", light);
				phongShader.send(camera, cameraController.modelTrans());
				model->draw(phongShader);
			});
		}
		else {
			shader([&](Shader& shader) {
				//shader.send(camera);
				model->draw(shader);
			});
		}
	}

	float width() const {
		return model->width();
	}

	float height() const {
		return model->height();
	}

	LightSource calculateLight(const Camera& camera) {
		LightSource l;
		glm::vec3 pos = camera.getPosition();
		l.position = glm::vec4(pos, 1);
		l.ambient = glm::vec4(0);
		l.diffuse = glm::vec4(1);
		l.specular = glm::vec4(1);
		l.transform = true;
		l.on = true;

		return l;
	}

	mat4 getViewport() {
		float w = scene.width() / 2.0f;
		float h = scene.height() / 2.0f;

		return mat4(
			vec4(w, 0, 0, 0),
			vec4(0, h, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(w, h, 0, 1)
		);
	}

private:
	Shader shader;
	Shader phongShader;
	Model* model;
	const CameraController& cameraController;
	Scene& scene;
};

class Floor {
public:
	Floor(const Camera& camera)
		:camera(camera){
		measurements.width = measurements.length = 8.0f;
	}

	void init() {
		initShader();
		shader.use();
		loadBrickTexture();
		loadLightMapTexture();
		shader.unUse();
		plane = new Plane(40, 40, 8.0f, 8.0f, 1, vec4(1), true);
	}

	void initShader() {
		shader.loadFromFile("shaders/lightMap.vert");
		shader.loadFromFile("shaders/lightMap.frag");
		shader.createAndLinkProgram();
	}

	void loadBrickTexture() {
		floorTex = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\wood_floor.jpg", 0, "image0", GL_RGBA8, GL_RGBA, glm::ivec2{ GL_REPEAT }, glm::ivec2{ GL_NEAREST });
		shader.sendUniform1i("image0", floorTex->id());
		
	}

	void loadLightMapTexture() {
		lightMap = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\floor_light_map.tga", 1);
		shader.sendUniform1i("image1", lightMap->id());

	}

	bool once = true;

	void draw() {

		shader([&](Shader& s) {
			s.send(camera);
			plane->draw(s);
		});
	}

	void drawWith(const Camera& camera) {

		shader([&](Shader& s) {
			s.send(camera);
			plane->draw(s);
		});
	}

	Mesurements dimensions() {
		return measurements;
	}

private:
	Plane* plane;
	const Camera& camera;
	Shader shader;
	Texture2D* floorTex;
	Texture2D* lightMap;
	Mesurements measurements;
};


class TestScene : public Scene {
public:
	TestScene(const char* title, Options ops) :Scene(title, ops) {	
		_vsync = true;
	}

	virtual void init() {
		using namespace glm;

		font = Font::Courier(15, 0, YELLOW);

		shader.loadFromFile("shaders/identity.vert");
		shader.loadFromFile("shaders/identity.frag");
		shader.createAndLinkProgram();

		camPos = new Sphere(0.1, 10, 10);
		

		cameraController = CameraController{ Mesurements{ float(_width), float(_height) }, Camera::ORBIT };
		spaceShip = new SpaceShip(cameraController, *this);
		spaceShip->init();


		floor = new Floor(cameraController.getCamera());
		floor->init();

		cameraController.setModelHeight(spaceShip->height());
		cameraController.setFloorMeasurement(floor->dimensions());

		cameraController.init();
		addKeyListener([&](const Key& key) {
			cameraController.processUserInput();
		});

		/*
		const Camera& cam1 = cameraController.getCamera();
		vec3 target = cam1.getPosition();
		vec3 eyes = target + vec3(0, 0, 3);
		cam2.setMode(Camera::SPECTATOR);
		cam2.lookAt(eyes, target, vec3(0, 1, 0));*/
		
		glClearColor(0, 0, 0, 0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	virtual void resized() override {
		projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
		cameraController.updateAspectRation(aspectRatio);
		const Camera& cam1 = cameraController.getCamera();
		cam2.perspective(cam1);
		font->resize(_width, _height);
	}

	virtual void display() override {
		using namespace glm;
		using namespace std;
		static std::stringstream sbr;
		
	//	font->render("FPS: " + to_string(fps), 10, _height - 10);
		const Camera& camera = cameraController.getCamera();
		vec3 target = camera.getPosition();
		vec3 eyes = target + vec3(0, 0, 3);


	//	glViewport(0, 0, _width / 2, _height );
		if (camera.getMode() == Camera::ORBIT) {
			spaceShip->draw();
		}
		floor->draw();
		font->render(msg(), 10, 20);

		/*glViewport(_width / 2, 0, _width / 2, _height );
		spaceShip->drawWith(cam2);
		floor->drawWith(cam2);
		shader([&]() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mat4 model = mat4::IDENTITY;
			model.translate(target.x, target.y, target.z);
			shader.send(cam2, model);
			camPos->draw(shader);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});*/
	}

	virtual void update(float elapsedTime) override {
		cameraController.update(elapsedTime);
	}

	std::string msg() {
		using namespace std;
		using namespace glm;
		stringstream sbr;
		
		const vec3& pos = cameraController.getCamera().getPosition();
		const vec3& vel = cameraController.getCamera().getVelocity();
		const float rotSpeed = cameraController.getCamera().getRotationSpeed();

		sbr << "FPS: " << fps << "\n\nCamera" << endl;
		sbr << setprecision(2);
		sbr << "\tPosition:" << pos << endl;
		sbr << "\tVelocity: " << vel << endl;
		sbr << "\tMode: " << cameraController.getCamera().modeAsString() << endl;
		sbr << "\tRotation speed: " << rotSpeed << endl;
		sbr << "\n\nMouse" << endl;
		sbr << "\tmouse pos: " << Mouse::get().pos << endl;
		sbr << "\trelative pos: " << Mouse::get().relativePos << endl;
		sbr << "\tSmoothing enabled" << endl;
		sbr << "\tSensitivity: 0.20" << endl;

		sbr << "\n\nPress H to display help";

		return sbr.str();
	}


private:
	Floor* floor;
	SpaceShip* spaceShip;
	CameraController cameraController;
	Camera cam2;
	Sphere* camPos;
	Font* font;
	std::ofstream fout;
	Shader shader;
	std::string mainMsg;
	glm::mat4 view;
	glm::mat4 projection;
};

