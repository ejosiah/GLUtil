#pragma once
#include <glm/glm.hpp>
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/Scene.h"
#include "include/ncl/gl/Shader.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class TesellationExample : public Scene {


public:
	using Scene::Scene;

	virtual void init() override {
	//	shader.loadFromFile(GL_VERTEX_SHADER, "shaders/pass_through.vert");
	//	shader.loadFromFile(GL_FRAGMENT_SHADER, "shaders/pass_through.frag");
	//	shader.loadFromFile(GL_TESS_CONTROL_SHADER, "shaders/pass_through.fcs");
	//	shader.loadFromFile(GL_TESS_EVALUATION_SHADER, "shader/pass_through.fes");
		shader.createAndLinkProgram();
		view = mat4(1);
		glClearColor(0, 0, 0, 0);

	}

	virtual void onResize(int w, int h) {
		projection = ortho(-0.2f, 1.2f, -0.2f, 1.2f, -1.f, 1.f);
	}

	virtual void display() {

	}

	virtual void update(float elapsedTime) {

	}
private:
	mat4 view;
	mat4 projection;
	Shader shader;
	ProvidedMesh* mesh;
};