#pragma once

#include <vector>
#include "include/ncl/gl/Scene.h"


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
		float sx = 2.0 / _width;
		float sy = 2.0 / _height;

		font->render("The Quick Brown Fox Jumps over the Lazy Dog", 0, _height - 20);
		//font->render("h", 0, 48);
	}

	virtual void resized() {

	}
private:
	Font* font;
	ProvidedMesh* point;
};