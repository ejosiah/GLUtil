#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/compute.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class MacBethChartScene : public Scene {
public:
	MacBethChartScene() :Scene("Compute Scene", 192 * factor, 128 * factor) {
		_useImplictShaderLoad = true;
		_requireMouse = false;
		//	_fullScreen = true;
	}

	void init() override {
		initQuad();

		image = new Image2D(_width, _height, GL_RGBA32F);

		shader("compute")([&](Shader& s) {
			s.sendUniform1i("factor", factor);
			image->computeMode();
			image->sendTo(s);
			glDispatchCompute(_width / (patchSize), _height / (patchSize), 1);
		});

		mat3(1, 1, 1, 1, 1, 1, 1, 1, 1) * vec3(0);
		mat4(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1) * vec4(0);
	}

	void initQuad() {
		Mesh mesh;
		mesh.positions = {
			{ -1.0f, -1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f },
		{ 1.0f,  1.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f }
		};
		mesh.uvs[0] = {
			{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
		};
		mesh.indices = { 0,1,2,0,2,3 };
		//	mesh.primitiveType = GL_TRIANGLE_FAN;
		quad = new ProvidedMesh(mesh);

	}

	void display() override {

		shader("plain")([&](Shader& s) {
			image->renderMode();
			image->sendTo(s);
			quad->draw(s);
		});
	}

	void processInput(const Key& key) override {

	}

	void resized() override {

	}

private:
	Image2D * image;
	ProvidedMesh* quad;
	static int factor;
	static int patchSize;
};

int MacBethChartScene::factor = 4;
int MacBethChartScene::patchSize = 32;