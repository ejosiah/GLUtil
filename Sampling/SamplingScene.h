#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/sampling/Sampler.h"
#include <glm/glm.hpp>
#include <algorithm>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace sampling;

class SamplingScene : public Scene {
public:
	SamplingScene() :Scene("Sampling", Resolution::HD.height, Resolution::HD.height) {
		useImplictShaderLoad(true);
		_requireMouse = false;
	}

	void init() override {
		Mesh m;
		m.primitiveType = GL_POINTS;
		
		for (int i = 0; i < numPoints; i++) {
			m.positions.push_back(vec3(0));
			m.colors.push_back(vec4(0));
		}
		m.positions.push_back(vec3(0));
		points = new ProvidedMesh(m);
	}

	void display() override {
		glViewport(0, _height/2, _width / 2, _height / 2);
		shader("sampling")([&](Shader& s) {
			send(cam);
			shade(points);
		});
	}

	void update(float dt) override {
		static float t = 0;
		t += dt;

		if (t >= 0.1) {
			points->update2<vec3>(0, [&](vec3* p) {
				for (int i = 0; i < numPoints; i++) {
					vec2 rp = sampler.get2D();
					(p + i)->x = rp.x;
					(p + i)->y = rp.y;
				}

				});
			t = 0;
		}
	}

	void resized() override {
		cam.projection = ortho(0, 1, 0, 1);
	}

private:
	RandomSampler sampler;
	ProvidedMesh* points;
	const int numPoints = 1000;
};