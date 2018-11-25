#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/compute.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ComputeScene : public Scene {
public:
	ComputeScene():Scene("Compute Scene", 1024, 1024){
		_useImplictShaderLoad = true;
		_requireMouse = false;
	//	_fullScreen = true;
	}

	void init() override {
		initQuad();

		image = new Image2D(_width, _height, GL_RGBA32F);
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
		shader("lerp")([&](Shader& s) {
			image->computeMode();
			image->sendTo(s);
			glDispatchCompute(_width/32, _height/32, 1);
		});

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
	Image2D* image;
	ProvidedMesh* quad;
};