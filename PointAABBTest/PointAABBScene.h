#pragma once

#include <iomanip>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include "../GLUtil/include/ncl/geom/Ray.h"
#include "../GLUtil/include/glm/vec_util.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace geom;
using namespace bvol;

class InputManager : public _3DMotionEventHandler {
public:
	InputManager(vec3* p) : point(p) {}

	virtual void onMotion(const _3DMotionEvent& event) override {
		*point += 0.0001f * event.translation;
		moved = true;
	}

	virtual void onNoMotion() override {
		moved = false;
	}

public:
	vec3* point;
	bool moved = false;
};


class PointAABBScene : public Scene {
public:
	PointAABBScene() :Scene("Ray Triangle intersection") {
		p = vec3(1.2);
		inputMgr = new InputManager(&p);
		_motionEventHandler = inputMgr;
	}

	virtual void init() override {
		font = Font::Arial(10);
		aabb = new AABBShape(AABB{ min, max });
		minVector = new Vector(min);
		maxVector = new Vector(max);
		dVector = new Vector(max - min, min);

		q = aabb->aabb.closestPoint(p);

		Mesh m;
		m.positions.push_back(p);
		m.positions.push_back(q);
		m.positions.push_back(aabb->min());
		m.positions.push_back(aabb->max());

		m.colors.push_back(BLACK);
		m.colors.push_back(RED);
		m.colors.push_back(BLUE);
		m.colors.push_back(RED);
		m.primitiveType = GL_POINTS;

		points = new ProvidedMesh(m);

		cam.view = lookAt({ 2.0f, 1.5f, 3.0f }, vec3(0), { 0, 1, 0 });

		glPointSize(5.0);
	}

	virtual void display() override {
		displayMessage();
		shader("flat")([&]() {
			send(cam);
			shade(aabb);
			shade(points);
			shade(dVector);
		});
	}

	virtual void update(float dt) override {
		if (inputMgr->moved) {
			q = aabb->aabb.closestPoint(p);
			d = sqrtf(aabb->aabb.sqDistance(p));
			points->update2<vec3>(0, [&](vec3* px) {
				*px = p;
				*(px + 1) = q;
			});
			if (inputMgr->moved && p == q) {
				points->update2<vec4>(VAOObject::Color, [&](vec4* c) {
					*c = BLUE;
				});
			}
			else {
				points->update2<vec4>(VAOObject::Color, [&](vec4* c) {
					*c = BLACK;
				});
			}
		}
	}

	void displayMessage() {
		stringstream ss;

		ss << "p: " << p << endl;
		ss << "q: " << q << endl;
		ss << "p distance from AABB: " << d << endl;

	//	font->resize(_width / 2, _height);
		font->render(ss.str(), 10, _height - 10);
	}

	virtual void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 0.3f, 100.f);
	}

private:
	vec3 p;
	vec3 q;
	float d;
	ProvidedMesh* points;
	Font* font;
	InputManager* inputMgr;
	AABBShape* aabb;
	Vector* minVector;
	Vector* maxVector;
	Vector* dVector;
	vec3 min = vec3(-1);
	vec3 max = vec3(1);
};