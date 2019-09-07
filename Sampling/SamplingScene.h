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

static const float PI = pi<float>();
static const float PI_OVER4 = PI / 4.f;
static const float PI_OVER2 = PI / 2.f;
static const float TWO_PI = two_pi<float>();

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

		if (useCosine) return cosineSampleHemisphere(u);

		float r = 1 - u.x * u.x;
		float phi = two_pi<float>() * u.y;
		return vec3{
			cos(phi) * sqrt(r),
			u.x,
			sin(phi) * sqrt(r)
		};
	}

	vec2 sampleDisk(const vec2& u) {
		float r = sqrt(u.x);
		float theta = TWO_PI * u.y;
		return vec2(r * cos(theta), r * sin(theta));
	 }

	vec2 concentricSampleDisk(const vec2& u) {
		auto uOffset = 2.f * u - vec2(1);

		if (uOffset.x == 0 && uOffset.y == 0) return vec2(0);

		float theta, r;
		if (abs(uOffset.x) > abs(uOffset.y)) {
			r = uOffset.x;
			theta = PI_OVER4 * (uOffset.y / uOffset.x);
		}
		else {
			r = uOffset.y;
			theta = PI_OVER2 - PI_OVER4 * (uOffset.x / uOffset.y);
		}
		return r * vec2(cos(theta), sin(theta));
	}

	vec3 cosineSampleHemisphere(const vec2& u) {
		vec2 d = concentricSampleDisk(u);
		float y = sqrt(std::max(0.f, 1 - d.x * d.x - d.y * d.y));
		return { d.x, y, d.y };
	}

	void processInput(const Key& key) override {
		if (key.value() == 'h' && key.released()) useCosine = !useCosine;
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
	bool useCosine = true;
	
};