#pragma once

#include <iomanip>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include "../GLUtil/include/ncl/geom/Ray.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace geom;

class InputManager : public _3DMotionEventHandler {
public:
	InputManager(bool& p, bool& q, Line& l): p(p), q(q), l(l) {}

	virtual void onMotion(const _3DMotionEvent& event) override {
		vec3 trans = 0.0001f * event.translation; // *vec3{ 1, -1, 1 };
		if (p) l.p0 += trans;
		if (q) l.p1 += trans;
		moved = true;
	}

	virtual void onNoMotion() override {
		moved = false;
	}

public:
	bool& p;
	bool& q;
	Line& l;
	bool moved = false;
};


class RayTriangleIntersectionScene : public Scene {
public:
	RayTriangleIntersectionScene() :Scene("Ray Triangle intersection") {
		t = { vec3{ 0, 0, 0 },  vec3{ 4, 0, -4 },  vec3{ -4, 0, -4 } };
		l = { {0, 1, 0}, {0, -2, 0} };
		l = { { 0.34f, 1.6f, -2.7f } ,{ 0.29f, -1.5f, -2.0f} };
		inputMgr = new InputManager(p, q, l);
		_motionEventHandler = inputMgr;
	}

	virtual void init() override {
		font = Font::Arial(10);
		Mesh m0;
		m0.positions = { t.a, t.b, t.c };
		m0.colors = vector<vec4>(3, RED);
		m0.primitiveType = GL_TRIANGLES;

		tMesh = new ProvidedMesh(m0);

		Mesh m1;
		m1.positions = { l.p0, l.p1 };
		m1.colors = { BLACK, BLACK };
		m1.primitiveType = GL_LINES;
		lMesh = new ProvidedMesh(m1);

		auto w = l.p0 - t.a;
		auto x = l.p0 - l.p1;
		auto y = t.b - t.a;
		auto z = t.c - t.a;

		M = mat3(
			x.x, y.x, z.x,
			x.y, y.y, z.y,
			x.z, y.z, z.z
		);

		Mesh m2;
		ap = new Vector(x);
		ab = new Vector(y);
		ac = new Vector(z);
		ww = new Vector(w);
		auto tvw = inverse(M) * w;
		auto tt = tvw.x;
		auto p = l.p0 - x * tt;
		m2.positions = { l.p0, l.p1, t.a, t.b, t.c, p };
		m2.colors = vector<vec4>(6, BLACK);
		m2.colors[0] = RED;
		m2.colors[m2.colors.size() - 1] = GREEN;
		m2.primitiveType = GL_POINTS;
		points = new ProvidedMesh(m2);
		glPointSize(5.0);
	}

	virtual void display() override {
		glViewport(0, 0, _width / 2, _height);
		displayMessage();
		shader("flat")([&]() {
			cam.view = lookAt({ -2, 2, 5 }, vec3(0), { 0, 1, 0 });
			send(cam);
		//	shade(tMesh);
			shade(lMesh);
			shade(points);
			shade(ww);
			shade(ap);
			shade(ab);
			shade(ac);
		});

		shader("flat")([&]() {
			glViewport(_width / 2, 0, _width / 2, _height);
			cam.view = lookAt({ 0, 5, 5 }, vec3(0), { 0, 0, 1 });
			send(cam);
			shade(tMesh);
			shade(lMesh);
			shade(points);
		});
	}

	virtual void update(float dt) override {
		if (Keyboard::get().P.pressed() & !p) {
			p = true;
			q = false;
			switched = true;
		}
		else if (Keyboard::get().Q.pressed() & !q) {
			q = true;
			p = false;
			switched = true;
		}
		if (switched) {
			points->update2<vec4>(VAOObject::Color, [&](vec4* color) {
				if (p) {
					*color = RED;
					*(color + 1) = BLACK;
				}
				else if (q) {
					*(color) = BLACK;
					*(color + 1) = RED;
				}
			});
			switched = false;
		}

		if (inputMgr->moved) {
			points->update2<vec3>(VAOObject::Position, [&](vec3* line) {
				*line = l.p0;
				*(line + 1) = l.p1;
			});
			lMesh->update2<vec3>(VAOObject::Position, [&](vec3* line) {
				*line = l.p0;
				*(line + 1) = l.p1;
			});
		}
	}

	void displayMessage() {
		stringstream ss;
		ss << "Line: \n";
		ss << setprecision(2);
		ss << "\tP [" << l.p0.x << ", " << l.p0.y << ", " << l.p0.z << "]\n";
		ss << "\tQ [" << l.p1.x << ", " << l.p1.y << ", " << l.p1.z << "]\n";
		ss << "Triangle: \n";
		ss << "\tA [" << t.a.x << ", " << t.a.y << ", " << t.a.z << "]\n";
		ss << "\tB [" << t.b.x << ", " << t.b.y << ", " << t.b.z << "]\n";
		ss << "\tC [" << t.c.x << ", " << t.c.y << ", " << t.c.z << "]\n";

		font->resize(_width / 2, _height);
		font->render(ss.str(), 10, 10);
	}

	virtual void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 0.3f, 100.f);
	}

private:
	Triangle t;
	Line l;
	bool p = true;
	bool q = false;
	bool switched = false;
	ProvidedMesh* tMesh;
	ProvidedMesh* lMesh;
	ProvidedMesh* points;
	Font* font;
	InputManager* inputMgr;
	mat3 M;
	Vector* ap;
	Vector* ab;
	Vector* ac;
	Vector* ww;
};