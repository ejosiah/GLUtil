#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iterator>

using namespace ncl;
using namespace gl;
using namespace glm;
using namespace std;

class TessellationScene : public Scene {

	enum class TessellationType {
		QUADS, TRIANGLES, ISOLINES
	};

	enum class TessellationSpacing {
		EQUAL_SPACING, FRACTIONAL_EVEN_SPACING, FRACTIONAL_ODD_SPACING
	};

	enum class State {
		NONE,
		TESS_LEVEL_SELECT,
		TESS_TYPE_SELECT,
		TESS_SPACING_SELECT
	};

public:
	TessellationScene() :Scene("Tessellation Scene", 1000, 1000) {
		useImplictShaderLoad(true);
		_requireMouse = false;
	}

	virtual void init() override {
		setBackGroundColor(BLACK);
		setForeGroundColor(WHITE);
		createQuatPatch();
		createTrianglePatch();
		createIsolinePatch();
	}

	void createQuatPatch() {
		Mesh mesh;
		
		mesh.positions.push_back(vec3(-0.5, -0.5, 0));
		mesh.positions.push_back(vec3(0.5, -0.5, 0));
		mesh.positions.push_back(vec3(0.5, 0.5, 0));
		mesh.positions.push_back(vec3(-0.5, 0.5, 0));
		mesh.colors = vector<vec4>(4, WHITE);
		mesh.primitiveType = GL_PATCHES;

		quadPatch = unique_ptr<ProvidedMesh>{ new ProvidedMesh(mesh) };
	}

	void createIsolinePatch() {
		Mesh mesh;

		mesh.positions.push_back({ -0.5, -0.5, 0.0 });
		mesh.positions.push_back({ 0.5, -0.5, 0.0 });

		mesh.positions.push_back({ -0.5, -0.25, 0.0 });
		mesh.positions.push_back({ 0.5, -0.25, 0.0 });

		mesh.positions.push_back({ -0.5, 0.0, 0.0 });
		mesh.positions.push_back({ 0.5, 0.0, 0.0 });

		mesh.positions.push_back({ -0.5, 0.25, 0.0 });
		mesh.positions.push_back({ 0.5, 0.25, 0.0 });

		mesh.positions.push_back({ -0.5, 0.5, 0.0 });
		mesh.positions.push_back({ 0.5, 0.5, 0.0 });

		
		mesh.colors = vector<vec4>(10, WHITE);
		mesh.primitiveType = GL_PATCHES;

		isolinePatch = unique_ptr<ProvidedMesh>{ new ProvidedMesh(mesh) };
	}

	void createTrianglePatch() {
		Mesh mesh;
		mesh.positions = { {-0.5, -0.5, 0.0}, {0.5, -0.5, 0.0}, {0.0, 0.5, 0.0} };
		mesh.colors = vector<vec4>(3, WHITE);
		mesh.primitiveType = GL_PATCHES;
		trianglePatch = unique_ptr<ProvidedMesh>{ new ProvidedMesh(mesh) };
	}

	virtual void display() override {
		printMenue();
		printState();
		renderPatch();
	}

	void renderPatch() {
		switch (currentType) {
		case TessellationType::QUADS:
			renderQuadPatch();
			break;
		case TessellationType::TRIANGLES:
			renderTrianglePatch();
			break;
		case TessellationType::ISOLINES:
			renderIsolinePatch();
			break;
		}

	}

	void renderQuadPatch() {
		shader(patch("quad"))([&](Shader& s) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, tessLevelOuter);
			glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, tessLevelInner);
			cam.model = mat4(1);
			cam.projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			send(cam);
			shade(quadPatch.get());
		});
	}

	void renderTrianglePatch() {

		shader("triangle")([&](Shader& s) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPatchParameteri(GL_PATCH_VERTICES, 3);
			glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, tessLevelOuter);
			glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, tessLevelInner);
			cam.model = mat4(1);
			cam.projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			send(cam);
			shade(trianglePatch.get());
		});
	}

	void renderIsolinePatch() {

		shader("isoline")([&](Shader& s) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPatchParameteri(GL_PATCH_VERTICES, 2);
			glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, tessLevelOuter);
			glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, tessLevelInner);
			cam.model = mat4(1);
			cam.projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			send(cam);
			shade(isolinePatch.get());
		});
	}

	string patch(const string& name) {
		switch (currentSpacing)
		{
		case TessellationSpacing::EQUAL_SPACING:
			return name;
		case TessellationSpacing::FRACTIONAL_EVEN_SPACING:
			return name + "_even";
		case TessellationSpacing::FRACTIONAL_ODD_SPACING:
				return name + "_odd";
		default:
			return name;
		}
	}

	virtual void processInput(const Key& key) override {
		auto min = currentType == TessellationType::TRIANGLES ? 1 : 2;
		if (key.released()) {
			switch (currentState) {
			case State::NONE:
				switch (key.value()) {
				case '1':
					currentState = State::TESS_LEVEL_SELECT;
					break;
				case '2':
					currentState = State::TESS_TYPE_SELECT;
					break;
				case '3':
					currentState = State::TESS_SPACING_SELECT;
					break;
				}
				break;
			case State::TESS_LEVEL_SELECT:			
				switch (key.value()) {
				case 'w':
					tessLevelOuter[0]++;
					break;
				case 's':
					tessLevelOuter[0]--;
					break;
				case 'e':
					tessLevelOuter[1]++;
					break;
				case 'd':
					tessLevelOuter[1]--;
					break;
				case 'r':
					tessLevelOuter[2]++;
					break;
				case 'f':
					tessLevelOuter[2]--;
					break;
				case 't':
					tessLevelOuter[3]++;
					break;
				case 'g':
					tessLevelOuter[3]--;
					break;
				case 'y':
					tessLevelInner[0]++;
					break;
				case 'h':
					tessLevelInner[0]--;
					break;
				case 'u':
					tessLevelInner[1]++;
					break;
				case 'j':
					tessLevelInner[1]--;
					break;
				case '1':
					currentState = State::NONE;
					break;
				}
				replace_if(begin(tessLevelOuter), end(tessLevelOuter), [&](auto&& x) { return x < min; }, min);
				replace_if(begin(tessLevelInner), end(tessLevelInner), [&](auto&& x) { return x < min; }, min);
				break;
			case State::TESS_TYPE_SELECT:
				switch (key.value()) {
				case '2':
					currentType = TessellationType::QUADS;
					replace_if(begin(tessLevelOuter), end(tessLevelOuter), [](auto&& x) { return x < 2; }, 2);
					replace_if(begin(tessLevelInner), end(tessLevelInner), [](auto&& x) { return x < 2; }, 2);
					break;
				case '3':
					currentType = TessellationType::TRIANGLES;
					break;
				case '4':
					currentType = TessellationType::ISOLINES;
					break;
				case '1':
					currentState = State::NONE;
					break;
				}
				break;
			case State::TESS_SPACING_SELECT:
				switch (key.value()) {
				case '2':
					currentSpacing = TessellationSpacing::EQUAL_SPACING;
					break;
				case '3':
					currentSpacing = TessellationSpacing::FRACTIONAL_EVEN_SPACING;
					break;
				case '4':
					currentSpacing = TessellationSpacing::FRACTIONAL_ODD_SPACING;
					break;
				case '1':
					currentState = State::NONE;
				}
				break;
			}
		}
	}

	void printMenue() {
		switch (currentState) {
		case State::NONE:
			sFont->render("Tessellation Level: 1\tTessellation Type: 2\tSpacing: 3", 20, 20);
			break;
		case State::TESS_LEVEL_SELECT:
			sFont->render("Tess Level outer: [w/s, e/d, r/f, t/g]\tTess Level Inner: [y/h, u/j]\texit: 1", 20, 20);
			break;
		case State::TESS_TYPE_SELECT:
			sFont->render("Quad: 2\tTriangle: 3\tIsoline: 4\texit: 1", 20, 20);
			break;
		case State::TESS_SPACING_SELECT:
			sFont->render("Equal Spacing: 2\tFractional Event Spacing: 3\tFractional Odd Spacing: 4\texit: 1", 20, 20);
		}
	}

	void printState() {
		string tessType;
		switch (currentType) {
		case TessellationType::QUADS:
			tessType = "Quad";
			break;
		case TessellationType::TRIANGLES:
			tessType = "Triangle";
			break;
		case TessellationType::ISOLINES:
			tessType = "Isolines";
			break;
		}
		string spacing;
		switch (currentSpacing) {
		case TessellationSpacing::EQUAL_SPACING:
			spacing = "equal spacing";
			break;
		case TessellationSpacing::FRACTIONAL_EVEN_SPACING:
			spacing = "fractional event spacing";
			break;
		case TessellationSpacing::FRACTIONAL_ODD_SPACING:
			spacing = "fractional odd spacing";
			break;
		}
		stringstream ss;
		ss << "Tessellation Type:\t\t\t" << tessType << "\nTessellation Spacing:\t\t" << spacing << endl;
		if (currentType == TessellationType::TRIANGLES) {
			ss << "tessLevelOuter:\t\t\t\t[" << tessLevelOuter[0] << ", " << tessLevelOuter[1] << ", ";
			ss << tessLevelOuter[2] << "]" << endl;
			ss << "tessLevelInner:\t\t\t\t[" << tessLevelInner[0] << "]";
		}
		else {
			
			ss << "tessLevelOuter:\t\t\t\t[" << tessLevelOuter[0] << ", " << tessLevelOuter[1] << ", ";
			ss << tessLevelOuter[2] << ", " << tessLevelOuter[3] << "]" << endl;
			ss << "tessLevelInner:\t\t\t\t[" << tessLevelInner[0] << ", " << tessLevelInner[1] << "]";
		}


		sFont->render(ss.str(), 20, height() - 100);
	}

private:
	float tessLevelOuter[4]{ 4.0f, 4.0f, 4.0f, 4.0f };
	float tessLevelInner[2]{ 4.0f, 4.0f };
	float outer[3] = { 6, 5, 8 };
	float inner[1]{ 5 };
	unique_ptr<ProvidedMesh> quadPatch;
	unique_ptr<ProvidedMesh> trianglePatch;
	unique_ptr<ProvidedMesh> isolinePatch;
	TessellationType currentType = TessellationType::QUADS;
	TessellationSpacing currentSpacing = TessellationSpacing::EQUAL_SPACING;
	State currentState = State::NONE;
};