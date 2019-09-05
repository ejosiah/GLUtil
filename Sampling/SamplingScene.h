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
	SamplingScene() :Scene("Sampling", Resolution::QHD.height, Resolution::QHD.height) {
		useImplictShaderLoad(true);
		_requireMouse = false;
		animate = false;
	}

	void init() override {
		Mesh m;
		m.primitiveType = GL_POINTS;
		lightModel.colorMaterial = true;
		
		for (int i = 0; i < numPoints; i++) {
			m.positions.push_back(vec3(0));
			m.colors.push_back(vec4(0));
		}
		m.positions.push_back(vec3(0));
		points = new ProvidedMesh(m);
		generatePoints();
		initializeHemisphereSampling();
	}
	
	void initializeHemisphereSampling() {
		
		dw.resize(50);
		generate(dw.begin(), dw.end(), [&]() { return new Vector(sampleHemisphere() * 3.0f, vec3(0));  });

		hemisphere = new Hemisphere(1, 50, 50, RED);
	}

	void display() override {
	//	glViewport(0, _height/2, _width / 2, _height / 2);
		//shader("sampling")([&](Shader& s) {
		//	send(cam);
		//	shade(points);
		//});

		renderHemisphereSampling();
	}

	void renderHemisphereSampling() {
		cam.view = lookAt({ 0, 3, 10 }, vec3(0), { 0, 1, 0 });
		cam.projection = perspective(half_pi<float>() / 2.0f, aspectRatio, 0.1f, 100.0f);
		light[0].on;
		light[0].position = { 0, 0, 1, 0 };

		shader("default")([&](Shader& s) {
			send(light[0]);
			send(cam);
			send(lightModel);
			shade(hemisphere);
			for (auto v : dw) shade(v);
		});
	}

	void update(float dt) override {
		if (animate) {
			static float t = 0;
			t += dt;

			if (t >= 0.1) {
				generatePoints();
				t = 0;
			}
		}
	}

	void generatePoints() {
		points->update2<vec3>(0, [&](vec3* p) {
			for (int i = 0; i < numPoints; i++) {
				vec2 rp = sampler.get2D();
				(p + i)->x = rp.x;
				(p + i)->y = rp.y;
			}

			});
	}

	void resized() override {
		cam.projection = ortho(0, 1, 0, 1);
	}

	vec3 sampleHemisphere() {
		vec2 u = rngSampler.get2D();
		float r = 1 - u.x * u.x;
		float phi = two_pi<float>() * u.y;
		return vec3{
			cos(phi) * sqrt(r),
			u.x,
			sin(phi) * sqrt(r)
		};
	}

private:
	bool animate;
	const int numPoints = 1000;
	StratifiedSampler sampler{ 20, 50, true };
	RandomSampler rngSampler;
	ProvidedMesh* points;
	Hemisphere* hemisphere;
	vector<Vector*> dw;
	LightModel lightModel;
	
};