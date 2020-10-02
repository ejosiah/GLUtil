#pragma once

#include <memory>
#include <tuple>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/glm/vec_util.h"
#include "../fsim/fields.h"
#include "FieldObject.h"
#include "calculus.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

class CustomScalaField : public ScalarField {
public:
	virtual ~CustomScalaField() {};

	virtual double sample(const vec3& p) const override {
		auto [x, y, z] = toTuple(p);
		z = half_pi<float>();
		return sin(x) * sin(y) * sin(z);
	//	return x * x + x * y * sin(z) - y * z;
	}
	
	virtual glm::vec3 gradient(const vec3& p) const override {
		auto [x, y, z] = toTuple(p);
		z = half_pi<float>();
		return { cos(x) * sin(y) * sin(z), sin(x) * cos(y) * sin(z), sin(x) * sin(y) *cos(z)};
		//return {
		//	2 * x + y * sin(z),
		//	x * sin(z) - z,
		//	x * y * cos(z) - y
		//};
	}

	virtual double laplacian(const glm::vec3& p) const {
		auto [x, y, z] = toTuple(p);
		z = half_pi<float>();
		return  
			- sin(x) * sin(y) * sin(z)
			- sin(x) * sin(y) * sin(z)
			- sin(x) * sin(y) * sin(z);
	//	return 2 - x * y * sin(z);
	}
private:

};

class CustomVectorField : public VectorField {
public:
	CustomVectorField() {

	}

	virtual ~CustomVectorField() {

	}

	virtual glm::vec3 sample(const glm::vec3& p) const {
		auto [x, y, z] = toTuple(p);
		return {
			sin(x) * sin(y),
			sin(y) * sin(z),
			sin(z) * sin(x)
		};
	}
	std::function<float(void)> rng = rngReal(0, 1);
	virtual double div(const glm::vec3& p) const {
		auto [x, y, z] = toTuple(p);
		z = half_pi<float>();
		return
			  cos(x) * sin(y)
			+ cos(y) * sin(z)
			+ cos(z) * sin(x);

	}

	virtual glm::vec3 curl(const glm::vec3& p) const {
		return vec3{ 0 };
	}

};

constexpr int numPoints = 10000;
constexpr int WIDTH =  1500;
constexpr int HEIGHT = 1000;


class VectorCalculusScene : public Scene {
public:
	VectorCalculusScene() :Scene("Vector Calculus", WIDTH, HEIGHT) , logger(Logger::get("vc")) {
		useImplictShaderLoad(true);
		_requireMouse = true;
	//	camInfoOn = true;
		_modelHeight = 20;
		addShader("canvas", GL_VERTEX_SHADER, identity_vert_shader);
		addShader("canvas", GL_FRAGMENT_SHADER, texture_frag_shader);
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
	}

	void init() override {

		sField = std::make_unique<ScalaFieldObject>(*this, field, numPoints, 1, two_pi<float>());
		sField->init();

		gradiantField = unique_ptr<VectorFieldObject>{ new VectorFieldObject([&](auto v) { 
			vec3 res = field.gradient(v.xzy).xzy;
			return res;
			}, 100, 1, 100, vec3(10, 10, 10), 5, RED) };
		
		vField = VectorFieldObject{ [&](auto v) { return cvField.sample(v); } , 10, 10, 10, vec3(10), 2, RED };
		vField1 = std::make_unique<VectorFieldSceneObject>(*this, cvField);
		createSeparator();
		//createHeatMap();
		//creatLaplacianMap();
		createImageCanvas();
		lightModel.colorMaterial = true;
		initDefaultCamera();
		activeCamera().setMode(Camera::Mode::FLIGHT);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		setForeGroundColor(WHITE);
		setBackGroundColor(BLACK);
		fontColor(WHITE);
	}

	void createImageCanvas() {
		Mesh mesh;
		mesh.positions.push_back(vec3(0, 1, 0));
		mesh.positions.push_back(vec3(0, 0, 0));
		mesh.positions.push_back(vec3(1, 1, 0));
		mesh.positions.push_back(vec3(1, 0, 0));
		mesh.uvs[0].push_back(vec2(0, 1));
		mesh.uvs[0].push_back(vec2(0, 0));
		mesh.uvs[0].push_back(vec2(1, 1));
		mesh.uvs[0].push_back(vec2(1, 0));
		mesh.material.diffuse = vec4(1, 0, 0, 1);
		mesh.primitiveType = GL_TRIANGLE_STRIP;

		canvas = std::unique_ptr<ProvidedMesh>{ new ProvidedMesh(vector<Mesh>(1, mesh)) };
	}

	void createSeparator() {
		Mesh mesh;
		mesh.positions.push_back({ 0.67, 0, 0 });
		mesh.positions.push_back({ 0.67, 1.0, 0.0 });
		mesh.positions.push_back({ 0.67, 0.5, 0.0 });
		mesh.positions.push_back({ 1.0, 0.5, 0.0 });
		mesh.colors = vector<vec4>{ 4, WHITE };
		mesh.primitiveType = GL_LINES;

		separator = std::unique_ptr<ProvidedMesh>{ new ProvidedMesh(vector<Mesh>(1, mesh)) };
	}

	void display() override {
		sFont->render("fps: " + to_string(fps), 20, 20);
		//renderSeparator();
		//sField->render();
		//glDepthFunc(GL_ALWAYS);
		//renderGradiantField();
		//glDepthFunc(GL_LESS);
		renderVectorField();
	
	}

	void renderGradiantField() {
		light[0].on;
		light[0].position = { 0, 10, 0, 0 };
		float w = width() * 0.35f;
		float h = height() * 0.5;
		glViewportIndexedf(3, width() * 0.65f, 0, w, h);
		shader("flat")([&](Shader& s) {
			cam.view = lookAt({ 0, 2, 0 }, vec3(0), { 1, 0, 0 });
			//cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			cam.projection = perspective(half_pi<float>() / 2.0f, aspectRatio, 0.1f, 1000.0f);
			send(cam);
			send(light[0]);
			send(lightModel);
			gradiantField->draw(s);
		});
	}

	void renderVectorField() {
		light[0].on;
		light[0].position = { 0, 0, 1, 0 };
		lightModel.colorMaterial = true;
		shader("flat")([&](Shader& s) {
			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			cam.projection = perspective(half_pi<float>() / 2.0f, aspectRatio, 0.1f, 1000.0f);
			send(cam);
			send(light[0]);
			send(lightModel);
		//	send(activeCamera(), getActiveCameraController().modelTrans());
		//	vField.draw(s);
		});
		vField1->render();
	}

	void renderHeatMap() {

		float w = width() * 0.35f;
		float h = height() * 0.5;
		glViewportIndexedf(1, width() * 0.65f, height() - h, w, h);
		shader("image")([&](Shader& s) {
			cam.view = mat4(1);
			cam.model = mat4(1);
			cam.projection = ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
			send(cam);
			send("id", 1);
			send(heatMap.get());
			shade(canvas.get());
		});

	}

	void renderSeparator() {
		shader("flat")([&](Shader& s) {
			glLineWidth(10.0f);
			cam.view = mat4(1);
			cam.model = mat4(1);
			cam.projection = ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
			send(cam);
			shade(separator.get());
		});
	}

	void processInput(const Key& key) override {
		sField->processInput(key);
		vField1->processInput(key);
	}

	void update(float t) override {
		sField->update(t);
		vField1->update(t);
		angle += speed * t;
	}

	void resized() override {

	}

private:
	std::unique_ptr<ScalaFieldObject> sField;
	std::unique_ptr<VectorFieldObject> gradiantField;
	VectorFieldObject vField;
	std::unique_ptr <VectorFieldSceneObject> vField1;
	std::unique_ptr<ProvidedMesh> canvas;
	std::unique_ptr<ProvidedMesh> separator;
	std::unique_ptr<Texture2D> heatMap;
	std::unique_ptr<Texture2D> laplacianMap;
	Logger& logger;
	CustomScalaField field;
	CustomVectorField cvField;
	float angle = 0;
	float speed = 20;
	vec3 eyes{ 0, 3, 10 };
	float dz = 1;
};