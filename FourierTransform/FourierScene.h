#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/AABB.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class Axis : public Drawable {
public:
	Axis(Scene& scene, int size, int numTerms, int dir = 0, int spacing = 1, bool padLast = false, int lineWidth = 1, vec4 color = WHITE)
		:scene(scene), lineWidth(lineWidth)  {

		float padding = float(size) / numTerms;
		float gap = padding / spacing;
		_extraSpace = padLast ?  2 * gap : 0;
		size += _extraSpace;

		mat2 rotor = dir == 1? mat2(0, 1, -1, 0) : mat2(1);

		Mesh m;
		m.positions.push_back(vec3(0));
		m.positions.push_back(vec3(size, 0, 0));
		m.colors.push_back(color);
		m.colors.push_back(color);
		
		int numGrids = numTerms * spacing;
		numGrids = padLast ? numGrids + 2 : numGrids;
		for (int i = 1; i < numGrids; i++) {
			if (i%spacing == 0) {
				auto p0 = vec3{ i * gap, -10, 0 };
				auto p1 = vec3{ i * gap, 10, 0 };
				m.positions.push_back(p0);
				m.positions.push_back(p1);
			}
			else {
				auto p0 = vec3{ i * gap, -5, 0 };
				auto p1 = vec3{ i * gap, 5, 0 };
				m.positions.push_back(p0);
				m.positions.push_back(p1);
			}
			m.colors.push_back(color);
			m.colors.push_back(color);
		}

		for (auto& pos : m.positions) {
			pos = vec3(rotor * pos.xy, 0);
		}

		m.primitiveType = GL_LINES;
		line = new ProvidedMesh(m);

		m.clear();
		m.positions.push_back({ size, -5, 0 });
		m.positions.push_back({ size + 15, 0, 0 });
		m.positions.push_back({ size, 5, 0 });
		m.colors = vector<vec4>{ 0, color };
		m.primitiveType = GL_TRIANGLES;

		for (auto& pos : m.positions) {
			pos = vec3(rotor * pos.xy, 0);
		}
		head = new ProvidedMesh(m);
		font = Font::Arial(20, 0, color);
		_size = size;
		_numTerms = padLast ? numTerms + 0.5 : numTerms;
	}

	virtual void draw(Shader& shader) override {
		glLineWidth(lineWidth);
		shade(line);
		shade(head);
	}

	int _size;
	float _numTerms;
	int _spacing;
	int _extraSpace;

private:
	ProvidedMesh* line;
	ProvidedMesh* head;
	Font* font;
	Scene& scene;
	float lineWidth;

};




class FourierScene : public Scene {
public:
	FourierScene() :Scene("Fourier Transformation", Resolution::FHD) {
		bounds = geom::bvol::AABB{ vec3(0), {width(), height(), 0} };
	}

	void init() override {
		setBackGroundColor(BLACK);
		xAxis = new Axis(*this, 1000, 4, 0, 4, true);
		yAxis = new Axis(*this, 150, 2, 1);

		float sampleRate = 0.001;
		float samples = std::ceil((1 / sampleRate) * xAxis->_numTerms);
		float steps = (xAxis->_size)/samples;
		float freq = 3 * glm::two_pi<float>();
		Mesh m;
		float t = 0;
		for (int i = 0; i < samples; i++) {
			t += sampleRate;
			float y = yAxis->_size * sin(freq * t);
			m.positions.push_back(vec3(steps * i, y, 0));
			m.colors.push_back(GREEN);
		}
		m.primitiveType = GL_LINE_STRIP;
		points = new ProvidedMesh(m);
		glEnable(GL_MULTISAMPLE);
	}

	void display() override {
		shader("flat")([&] {
			cam.model = translate(mat4(1), {50, height() / 2, 0 });
			send(cam);
			shade(xAxis);
			shade(yAxis);
			shade(points);
		});
	}

	void resized() override {
		cam.projection = ortho(bounds.min().x, bounds.max().x, bounds.min().y, bounds.max().y);
	}

private:
	geom::bvol::AABB bounds;
	Axis* xAxis;
	Axis* yAxis;
	ProvidedMesh* points;
};