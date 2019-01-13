#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/glm/vec_util.h"

using namespace ncl;
using namespace gl;
using namespace std;
using namespace glm;

class MassSpring : public Compute {
public:
	MassSpring(GLuint buf, Shader* s) :
		Compute(vec3(1), {}, s), _buf(buf) {}

	void preCompute() override {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _buf);
	}

private:
	GLuint _buf;
};

class MassSpringScene : public Scene {
public:
	MassSpringScene() :Scene("Mass spring") {
		useImplictShaderLoad(true);
	}

	void init() override {
		plane = new Plane(4, 4, 1, 1, 0, WHITE);
		points = new ProvidedMesh(*plane, GL_POINTS, WHITE);

		Mesh m;
		m.positions.push_back(vec3(0));
		m.colors.push_back(RED);
		m.primitiveType = GL_POINTS;
		center = new ProvidedMesh(m);

		auto colorId = points->bufferFor(0, VAOObject::Color);
		massSpring = new MassSpring(colorId, &shader("compute"));

		massSpring->compute();

		auto posId = plane->bufferFor(0, VAOObject::Position);
		vec3* data = (vec3*)glMapNamedBuffer(posId, GL_READ_ONLY);
		auto begin = data;
		auto end = data + 26;
		for (int i = 0; i < 25; i++) {
			cout << *data << endl;
			++data;
		}
		//	colors = new TextureBuffer("colors", nullptr, 0, colorId);
		glUnmapNamedBuffer(posId);
		glBindVertexArray(0);

		cam.view = lookAt({ 0, 2, 0 }, vec3(0), { 0, 0, -1 });
		cout << plane->numVertices(0) << endl;
		glPointSize(10.0);
		setBackGroundColor(BLACK);
	}

	void display() override {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader("point")([&] {
			send(cam);
			glPointSize(10);
			shade(points);
			glPointSize(20);
			shade(center);
		});
		
		shader("flat")([&] {
			send(cam);
			shade(plane);
		});

	}

	void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 1.0f, 100.f);
	}

private:
	Plane * plane;
	Image2D* image;
	ProvidedMesh* points;
	ProvidedMesh* center;
	Compute* compute;
	TextureBuffer* colors;
	MassSpring* massSpring;
};