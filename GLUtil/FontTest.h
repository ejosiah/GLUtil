#pragma once

#include <vector>
#include "include/ncl/gl/Scene.h"

#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class FontTest : public Scene {
public:
	FontTest() : Scene("Font test", 500, 500) {}

	virtual void init() override {
		font = Font::Arial(20, 0, RED);
		Mesh mesh;
		mesh.positions.push_back(vec3(0));
		point = new ProvidedMesh(vector<Mesh>(1, mesh));
	//	glClearColor(0, 0, 0, 1);
		
	}

	virtual void display() override {
		CHECK_GL_ERRORS
		float sx = 2.0 / _width;
		float sy = 2.0 / _height;

		renderText(10, _height - 20, "FPS: " + std::to_string(fps));
		
		for (int i = 0; i < 10; i++) {
			font->render("The Quick Brown Fox Jumps over the Lazy Dog", 10, _height - (40 + i * 20));
		}
		//font->render("h", 0, 48);
	}

	virtual void resized() {
		
	}
private:
	Font* font;
	ProvidedMesh* point;
};