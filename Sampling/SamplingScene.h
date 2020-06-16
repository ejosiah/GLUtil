#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/sampling/Sampler.h"
#include "../GLUtil/include/ncl/sampling/sampling.h"
#include <glm/glm.hpp>
#include <algorithm>
#include "../GLUtil/include/ncl/ray_tracing/camera.h"
#include "../GLUtil/include/ncl/ray_tracing/ray.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace sampling;

static const float PI = pi<float>();
static const float PI_OVER4 = PI / 4.f;
static const float PI_OVER2 = PI / 2.f;
static const float TWO_PI = two_pi<float>();

namespace rt = ray_tracing;

class SamplingScene : public Scene {
public:
	SamplingScene() :Scene("Sampling", Resolution::HD.width, Resolution::HD.height) {
		useImplictShaderLoad(true);
		_requireMouse = false;
		animate = false;
	}

	void init() override {
		initDefaultCamera();
		activeCamera().lookAt({ 0, 10, 0 }, vec3(0), { 0, 0, 1 });
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
		teapot = new Teapot;

	//	rt::update(rtCam, *this);
	//	generateRay(vec2{ _width / 2, _height / 2 }, rtCam, ray);

		ray = {
			{0, 0, 0, 1},
			{0.234, 0.146, -0.961, 1},
			5.0f
		};

		float cos0 = dot(planeNormal, -vec3(ray.direction.xyz));

		v_ray = new Vector{ ray.direction.xyz * ray.tMax, ray.origin.xyz, 1.0, YELLOW };
		normal = new Vector{ planeNormal, vec3(0), 1.0, RED };
		activeCamera().lookAt({ 0, 3, 10 }, vec3(0), { 0, 1, 0 });
		

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void generateRay(vec2 pos, rt::Camera camera, rt::Ray& ray) {
		vec3 p = (camera.rasterToCamera * vec4(pos, 0, 1)).xyz;
		vec4 origin = vec4(vec3(0), 1);
		vec3 direction = normalize(p);

		ray.origin = camera.cameraToWorld * origin;
		direction = mat3(camera.cameraToWorld) * direction;
		ray.direction = vec4(normalize(direction), 1);
		ray.tMax = length(vec3(ray.origin)) * 0.5;
	}
	
	void initializeHemisphereSampling() {
		
		//dw.resize(50);
		//generate(dw.begin(), dw.end(), [&]() { return new Vector(sampleHemisphere() * 3.0f, vec3(0));  });
		
		auto points = randomSphere(50);
		for (auto point : points) dw.push_back(new Vector{ point, vec3(0) });

		hemisphere = new Hemisphere(1, 50, 50, vec4(1, 0, 0, 0.2));
		hemisphere->defautMaterial(false);
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
			send("gammaCorrect", false);
			send(light[0]);
		//	send(cam);
			send(activeCamera());
			send(lightModel);
			//for (auto v : dw) shade(v);

			//glEnable(GL_BLEND);
			//glDisable(GL_DEPTH_TEST);
			//glDepthMask(false);
			//shade(hemisphere);
			//glDisable(GL_BLEND);
			//glEnable(GL_DEPTH_TEST);
			//glDepthMask(true);
		//	shade(teapot);
			shade(v_ray);
			shade(normal);

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
	Teapot* teapot;
	bool useCosine = true;
	vec3 planeNormal = { -0.383, -0.238, 0.893 };
	rt::Ray ray;
	rt::Camera rtCam;
	Vector* v_ray;
	Vector* normal;
	
};