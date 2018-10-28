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
	ComputeScene():Scene("Compute Scene", 720, 720){
		_useImplictShaderLoad = true;
	//	_fullScreen = true;
	}

	void init() override {
		initQuad();
		color_a = as[0];
		color_b = bs[0];
		//glGenTextures(1, &scene_img);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, scene_img);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, _width, _height);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//image = new Image2D(64, 64);
		//compute = new Compute({ 64, 64, 1 }, { *image }, &shader("compute"));
		board = new CheckerBoard_gpu;
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
		/*shader("compute")([&](Shader& s) {
			send("a", color_a);
			send("b", color_b);
			image->computeMode();
			image->sendTo(s);
			glDispatchCompute(8, 8, 1);
		});*/
		//compute->run();
		board->compute();

		shader("plain")([&](Shader& s) {
		//	image->renderMode();

		//	image->sendTo(s);
			board->images().front().renderMode();
			board->images().front().sendTo(s);
			quad->draw(s);
		});
	}

	void processInput(const Key& key) override {
		if (key.value() == 'a' && key.released()) {
			color_a = as[nextA];
			board->updateColorA(color_a);
			nextA = (nextA + 1) % 5;
		}
		else if (key.value() == 'b' && key.released()) {
			color_b = bs[nextB];
			board->updateColorB(color_b);
			nextB = (nextB + 1) % 5;
		}
	}

	void resized() override {

	}

private:
	Image2D* image;
	GLuint scene_img;
	vec4 color_a;
	vec4 color_b;
	vec4 as[5] = { WHITE, RED, BLUE, GREEN, YELLOW };
	vec4 bs[5] = { BLACK, RED, BLUE, GREEN, YELLOW };
	int nextA = 1;
	int nextB = 1;
	CheckerBoard_gpu* board;
	ProvidedMesh* quad;
};