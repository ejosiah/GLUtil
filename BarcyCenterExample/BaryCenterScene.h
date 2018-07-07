#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <limits>
#include <iomanip>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace geom;
using namespace glm;

class BaryCenterScene : public Scene {
public:
	BaryCenterScene() :Scene("Bary Center test") {
		cam.model = glm::scale(mat4(1), { 0.125f, 0.125f, 0 });
		t = {  vec3{-2, -6, 0},  vec3{4, -2, 0},  vec3{-4, 4, 0} };
	//	t = { vec3{ -4, 0, 0 },  vec3{ 4, 0, 0 },  vec3{ 0, 6, 0 } };
		float fMax = numeric_limits<float>::max();
		point = { fMax, fMax, fMax };
		_requireMouse = true;
	}

	virtual void init() override {

		Mesh m;
		m.primitiveType = GL_LINE_LOOP;
		m.positions.push_back(t.a);
		m.positions.push_back(t.b);
		m.positions.push_back(t.c);
		m.colors.push_back(color);
		m.colors.push_back(color);
		m.colors.push_back(color);
		triangle = new ProvidedMesh(m);

		Mesh m2;
		m2.primitiveType = GL_POINTS;
		m2.positions.push_back(point);
		m2.positions.push_back(t.a);
		m2.positions.push_back(t.b);
		m2.positions.push_back(t.c);

		m2.colors.push_back(color);
		m2.colors.push_back(color);
		m2.colors.push_back(color);
		m2.colors.push_back(color);

		points = new ProvidedMesh(m2);

		glPointSize(5.0);

		addMouseClickListener([&](Mouse& mouse) {
			if (mouse.left.status == Mouse::Button::PRESSED) {
				float winZ = 0;
				float x = mouse.pos.x;
				float y = _height - mouse.pos.y;
				glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
				point = unProject(vec3(x, y, winZ), mat4(1), cam.projection, vec4(0, 0, _width, _height));
				point *= vec3(8, 8, 0);
				stringstream ss;
				ss << "selected point: (" << point.x << ", " << point.y << ", " << point.z << ")";
				Logger::get("BCenter").info(ss.str());


				points->update2<vec3>(VAOObject::Position, [&](vec3* points) {
					points->x = point.x;
					points->y = point.y;
				});

				vec4 pointColor = t.contains(point) ? RED : BLUE;
				font = pointColor == RED ? inFont : outFont;
				points->update2<vec4>(VAOObject::Color, [&](vec4* colors) {
					colors->r = pointColor.r;
					colors->g = pointColor.g;
					colors->b = pointColor.b;
					colors->a = pointColor.a;
				});
			}
		});

		inFont = Font::Arial(10, 0, RED);
		outFont = Font::Arial(10, 0, BLUE);
		font = Font::Arial();
	}

	virtual void display() override {
		stringstream ss;
		
		ss << std::setprecision(2) << "point coordinates: [ " << point.x << ", " << point.y <<  " ]";
		font->render(ss.str(), 10, _height - 20);
		shader("flat")([&]() {
			send(cam);
			shade(points);
			shade(triangle);
		});
	}

	virtual void resized() override {
		cam.projection = glm::ortho(-1, 1, -1, 1, 1, -1);
	}

private:
	Font * inFont;
	Font* outFont;
	Font* font;
	vec3 point;
	vec4 inTryColor{ 1, 0, 0, 1 };
	vec4 color{ 0 };
	Triangle t;
	ProvidedMesh* triangle;
	ProvidedMesh* points;
};