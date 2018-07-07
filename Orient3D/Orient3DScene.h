#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <limits>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace geom;
using namespace glm;

class InputManager : public _3DMotionEventHandler {
public:
	InputManager(vec3& A, vec3& B, vec3& C, vec3& D):A(A), B(B), C(C), D(D){}

	virtual void onMotion(const _3DMotionEvent& event) override {
		D += 0.0001f * event.translation; // *vec3{ 1, -1, 1 };
	}

	virtual void onNoMotion() override {
	
	}

private:
	vec3 & A, &B, &C, &D;
};

class Orient3DScene : public Scene {
public:
	Orient3DScene() :Scene("Orient 3D") {
		t = { A = vec3{ 0, 0, 0 },  B = vec3{ 4, 0, -4 },  vec3{ -4, 0, -4 } };
		A = vec3(0);
		B = { 4, 0, -4 };
		C = { -4, 0, -4 };
		
		D = { 0, 2, -2 };
		AD = A - D;
		BD = B - D;
		CD = C - D;
		BD_CD = cross(BD, CD);
		_requireMouse = true;
		inputMgr = new InputManager(A, B, C, D);
		_motionEventHandler = inputMgr;
	}

	virtual void init() override {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Mesh m;
		m.primitiveType = GL_LINE_LOOP;
		m.positions.push_back(t.a);
		m.positions.push_back(t.b);
		m.positions.push_back(t.c);
		m.colors.push_back(color);
		m.colors.push_back(color);
		m.colors.push_back(color);
		triangle = new ProvidedMesh(m);


		constructObjects();

		float t = dot(normalize(A - D), normalize(cross(B - D, C - D)));
		if (t < 0) {
			Logger::get("Orient3D").info("D is above the supporting plain of triangle ABC");
		}
		else if (t == 0) {
			Logger::get("Orient3D").info("D is coplainer with triangle ABC");
		}
		else {
			Logger::get("Orient3D").info("D is below the supporting plain of triangle ABC");
		}
		stringstream ss;
		ss << "angle between AD and BD X CD is " <<  degrees(glm::acos(t));
		Logger::get("Orient3D").info(ss.str());

		glPointSize(5.0);

		//addMouseClickListener([&](Mouse& mouse) {
		//	if (mouse.left.status == Mouse::Button::PRESSED) {
		//		float winZ = 0;
		//		float x = mouse.pos.x;
		//		float y = _height - mouse.pos.y;
		//		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		//		vec3 point = unProject(vec3(x, y, winZ), cam.view, cam.projection, vec4(0, 0, _width, _height));
		//		stringstream ss;
		//		ss << "selected point: (" << point.x << ", " << point.y << ", " << point.z << ")";
		//		Logger::get("BCenter").info(ss.str());


		//		points->update2<vec3>(VAOObject::Position, [&](vec3* points) {
		//			points->x = point.x;
		//			points->y = point.y;
		//			points->z = -2;
		//		});

		//		vec4 pointColor = BLUE;
		//		points->update2<vec4>(VAOObject::Color, [&](vec4* colors) {
		//			colors->r = pointColor.r;
		//			colors->g = pointColor.g;
		//			colors->b = pointColor.b;
		//			colors->a = pointColor.a;
		//		});
		//	}
		//});

		cam.view = lookAt({ 0, 1, 5 }, vec3(0), { 0, 1, 0 });
	//	cam.view = lookAt({ 0, 10, 0 }, vec3(0), { 0, 0, 1 });
	}

	virtual void display() override {
		shader("flat")([&]() {
			send(cam);
			shade(points);
			shade(triangle);
			shade(BD_CDVec);
			shade(ADVec);
			shade(BDVec);
			shade(CDVec);
		});
	}

	virtual void update(float dt) override {
		AD = A - D;
		BD = B - D;
		CD = C - D;
		BD_CD = cross(BD, CD);
		delete BD_CDVec;
		delete ADVec;
		delete BDVec;
		delete CDVec;
		points->update2<vec3>(VAOObject::Position, [&](vec3* p) {
			*p = D;
		});
		constructObjects();
	}

	void constructObjects() {
		Mesh m2;
		m2.primitiveType = GL_POINTS;
		m2.positions.push_back(D);
		m2.positions.push_back(A);
		m2.positions.push_back(B);
		m2.positions.push_back(C);

		m2.colors.push_back(color);
		m2.colors.push_back(color);
		m2.colors.push_back(color);
		m2.colors.push_back(color);

		BD_CDVec = new Vector(normalize(BD_CD), D, 0.2, GREEN);
		ADVec = new Vector(AD, D, 0.2, BLACK);
		BDVec = new Vector(BD, D, 0.2, RED);
		CDVec = new Vector(CD, D, 0.2, BLUE);

		points = new ProvidedMesh(m2);
	}

	virtual void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 0.3f, 100.f);
	}

private:
	vec3 A, B, C, D;
	vec3 AD, BD, CD, BD_CD;
	vec4 inTryColor{ 1, 0, 0, 1 };
	vec4 color{ 0 };
	Triangle t;
	ProvidedMesh* triangle;
	ProvidedMesh* points;
	ProvidedMesh* vectors;
	Vector*  BD_CDVec;
	Vector*  ADVec;
	Vector*  BDVec;
	Vector*  CDVec;
	InputManager* inputMgr;
};