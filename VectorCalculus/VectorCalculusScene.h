#pragma once
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../fsim/fields.h"
#include "FieldObject.h"
#include <memory>
#include "../GLUtil/include/glm/vec_util.h"
#include <tuple>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

class CustomScalaField : public ScalarField {
public:
	virtual ~CustomScalaField() {};

	virtual double sample(const vec3& p) const override {
		return pow(p.x, 2) * sin( 5 * p.y);
	}

	//virtual glm::vec3 gradient(const vec3& p) const override {

	//	return { 2 * p.x * sin(5 * p.z), 0, pow(p.x, 2) * 5 * cos(5 * p.x) };
	//}
	
	virtual glm::vec3 gradient(const vec3& p) const override {

		return { 2 * p.x * sin(5 * p.y), 0, pow(p.x, 2) * 5 * cos(5 * p.x) };
	}

	virtual double laplacian(const glm::vec3& p) const {
		return  (2 - p.y * p.y) + (p.x * p.x - 2);
	}
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
			5 * z * z,
			2 * x,
			x + 2 * y
		};
	}

	virtual double div(const glm::vec3& x) const {
		return 0.0;
	}

	virtual glm::vec3 curl(const glm::vec3& x) const {
		return vec3{ 0 };
	}

};

constexpr int numPoints = 10000;

class VectorCalculusScene : public Scene {
public:
	VectorCalculusScene() :Scene("Vector Calculus", 1500, 1000) , logger(Logger::get("vc")) {
		useImplictShaderLoad(true);
	//	_requireMouse = true;
	//	camInfoOn = true;
		_modelHeight = 20;
		addShader("canvas", GL_VERTEX_SHADER, identity_vert_shader);
		addShader("canvas", GL_FRAGMENT_SHADER, texture_frag_shader);
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
	}

	void init() override {
		sField = std::make_unique<ScalaFieldObject>(*this, field, numPoints, 4);
		sField->init();

		gradiantField = unique_ptr<VectorFieldObject>{ new VectorFieldObject(field, 50, 50, 0, 20, 5, RED) };
		vField = VectorFieldObject{ cvField, 10, 10, 10, 10, 2, RED};
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

	//void createHeatMap() {
	//	auto data = new GLubyte[numPoints * numPoints * 4];
	//	auto id = 0;
	//	sField->get<vec4>(0, VAOObject::Color, [&](vec4* itr) {
	//		for (auto i = 0; i < numPoints; i++) {
	//			for (auto j = 0; j < numPoints; j++) {
	//				int idx = (i * numPoints + j) * 4;
	//				auto color = *itr;
	//				data[idx] = color.r * 255;
	//				data[idx + 1] = color.g * 255;
	//				data[idx + 2] = color.b * 255;
	//				data[idx + 3] = color.a * 255;
	//				itr++;
	//			}
	//		}
	//		});
	//	heatMap = std::unique_ptr<Texture2D>{ new Texture2D(static_cast<void*>(data), numPoints, numPoints, "heatMap", 0) };
	//}

	//void creatLaplacianMap() {
	//	auto data = new GLubyte[numPoints * numPoints * 4];
	//	auto id = 0;
	//	float lower = numeric_limits<float>::max();
	//	float upper = numeric_limits<float>::min(); 
	//	float* laplacian = new float[numPoints * numPoints];
	//	sField->get<vec3>(0, VAOObject::Position, [&](vec3* itr) {
	//		for (auto i = 0; i < numPoints; i++) {
	//			for (auto j = 0; j < numPoints; j++) {
	//				float res = field.laplacian(*itr);
	//				lower = std::min(lower, res);
	//				upper = std::max(upper, res);
	//				laplacian[(i * numPoints + j)] = res;
	//				itr++;
	//			}
	//		}
	//	});
	//	lower = lower > 0 ? 0 : lower;
	//	for (auto i = 0; i < numPoints; i++) {
	//		for (auto j = 0; j < numPoints; j++) {
	//			float l = convertRange(laplacian[(i * numPoints + j)], lower, upper, 0, 1);
	//			int idx = (i * numPoints + j) * 4;
	//			auto color = vec4(l, l, l, 1);
	//			data[idx] = color.r * 255;
	//			data[idx + 1] = color.g * 255;
	//			data[idx + 2] = color.b * 255;
	//			data[idx + 3] = color.a * 255;
	//		}
	//	}
	//	laplacianMap = std::unique_ptr<Texture2D>{ new Texture2D(static_cast<void*>(data), numPoints, numPoints, "laplacianMap", 0) };
	//}

	float convertRange(float x, float old_min, float old_max, float new_min, float new_max) {
		double num = (new_max - new_min) * (x - old_min);
		double denum = old_max - old_min;

		return (num / denum + new_min);
	}

	void display() override {
		sFont->render("fps: " + to_string(fps), 20, 20);
	//	renderHeatMap();
		//renderSeparator();
		//sField->renderHeatMap();
		sField->renderField();
	//	sField->renderLaplacian();
		renderGradiantField();
	//	renderVectorField();
	
	}

	void renderScalaField() {
		light[0].on;
		light[0].position = { 0, 10, 0, 0 };

		
		//shader("vc")([&](Shader& s) {
		//	bool blendingOff = !glIsEnabled(GL_BLEND);
		//	bool depthTestOn = glIsEnabled(GL_DEPTH_TEST);
		//	if (blendingOff) glEnable(GL_BLEND);
		//	if (depthTestOn) glDisable(GL_DEPTH_TEST);
		//	glViewportIndexedf(0, 0, 0, width(), height());
		////	glViewportIndexedf(0, 0, 0, width() * 0.65f, height());
		//	cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
		//	cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
		//	cam.model = rotate(cam.model, -glm::half_pi<float>(), { 1.0f, 0, 0 });
		//	float ar = float(width() * 0.65) / height();
		//	cam.projection = perspective(half_pi<float>() / 2.0f, ar, 0.1f, 1000.0f);
		//	//send(light[0]);
		//	send(cam);
		////	send(lightModel);
		//	shade(sField.get());
		//	if (blendingOff) glDisable(GL_BLEND);
		//	if (depthTestOn) glEnable(GL_DEPTH_TEST);
		//});
		sField->render();
	}

	void renderGradiantField() {
		light[0].on;
		light[0].position = { 0, 10, 0, 0 };
		float w = width() * 0.35f;
		float h = height() * 0.5;
		glViewportIndexedf(3, width() * 0.65f, 0, w, h);
		shader("image")([&](Shader& s) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			cam.view = lookAt({ 0, 3, 0 }, vec3(0), {1, 0, 0});
			cam.model = mat4(1);
			cam.projection = perspective(half_pi<float>() / 2.0f, w/h, 0.1f, 100.0f);
			send(cam);
			send("id", 3);
			gradiantField->draw(s);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});
	}

	void renderVectorField() {
		light[0].on;
		light[0].position = { 0, 0, 1, 0 };
		lightModel.colorMaterial = true;
		shader("default")([&](Shader& s) {
			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			cam.model = rotate(cam.model, -glm::half_pi<float>(), { 1.0f, 0, 0 });
			cam.projection = perspective(half_pi<float>() / 2.0f, aspectRatio, 0.1f, 1000.0f);
			send(cam);
			send(light[0]);
			send(lightModel);
		//	send(activeCamera(), getActiveCameraController().modelTrans());
			vField.draw(s);
		});
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
	}

	void update(float t) override {
		sField->update(t);
	}

	void resized() override {

	}

private:
	std::unique_ptr<ScalaFieldObject> sField;
	std::unique_ptr<VectorFieldObject> gradiantField;
	VectorFieldObject vField;
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