#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/common.h"
#include "../GLUtil/include/ncl/gl/FrameBuffer.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "../GLUtil/include/ncl/units/units.h"
#include "../Physics/Floor.h"
#include "Weather.h"
#include "CloudUI.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;
using namespace unit;

const unsigned WIDTH = Resolution::QHD.width;
const unsigned HEIGHT = Resolution::QHD.height;

class CloudScene : public Scene {
public:
	CloudScene() :Scene{ "Perlin-Worley Clouds", WIDTH, HEIGHT } {
		camInfoOn = true;
	//	_hideCursor = false;
	//	_requireMouse = true;
		addShader("render", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("phong", GL_VERTEX_SHADER, phong_lfp_vert_shader);
		//		addShader("phong", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("phong", GL_FRAGMENT_SHADER, phong_lfp_frag_shader);
	//	addShader("clouds", GL_VERTEX_SHADER, identity_vert_shader);
	}

	void init() override {
		glDisable(GL_CULL_FACE);
		_modelHeight = 5.0f;
		bounds(vec3(-200), vec3(200));
		initDefaultCamera();
		activeCamera().perspective(60.0f, _width / _height, 10.0_cm, 100.0_km);
		activeCamera().collisionTestOff();
//		activeCamera().setPosition({ 3.26, 24.6, 13.1 });
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);

		noiseQuad = ProvidedMesh{ screnSpaceQuad(), false, dim.z };
		noiseQuad.defautMaterial(false);
		
		noiseTexture = new Texture3D{
			nullptr,
			dim.x,
			dim.y,
			dim.z,
			0,
			GL_RGBA32F,
			GL_RGBA,
			glm::vec3{ GL_REPEAT },
			glm::vec2{ GL_LINEAR },
			GL_FLOAT
		};

		int val;
		glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &val);
		logger.info("Max GL invocations: " + to_string(val));

		generateNoise();
		
		setBackGroundColor({0.70, 0.76, 0.83, 1.0});
		auto lightPos = vec3{ 0, 500, 10 };
		light[0].position = vec4{ lightPos, 1 };
		floor = new Floor(this, vec2(60.0_km));
		inner = new Hemisphere{ cloudMinMax.x, 20, 20, RED };
		outer = new Hemisphere{ cloudMinMax.y, 20, 20, GREEN };
		auto xform = translate(mat4(1), { 0, 0.5, 0 });
		xform = scale(xform, vec3(1));
		//auto xform = mat4(1);
		cube = Cube{ 1, WHITE, vector<mat4>{1, xform }, false };
		cubeAABB = aabbOutline(cube.aabb(), BLACK);
	//	cube = Cube{ 1, WHITE};
		sphere = Sphere{ 0.5 };
		cloudUI = new CloudUI{ weather, *this };
	}


	void generateNoise() {
		shader("noise")([&] {
			glBindImageTexture(image, noiseTexture->buffer(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			glDispatchCompute(workers.x, workers.y, workers.z);
		});

	}

	void display() override {
		cloudUI->render();
		//renderBounds();
		
	//	renderNoise();
		renderClouds();
		renderFloor();
		//shader("flat")([&] {
		//	send(activeCamera());
		//	shade(cubeAABB);
		//	send(activeCamera(), translate(mat4(1), cube.aabbMin()));
		//	shade(sphere);
		//	send(activeCamera(), translate(mat4(1), cube.aabbMax()));
		//	shade(sphere);
		//});	
	}

	void renderNoise() {
		shader("render")([&] {
			glBindTextureUnit(0, noiseTexture->buffer());
			send("dt", Timer::get().timeSinceStart());
			send("numSlices", int(dim.z));
			send("slice", 70.0f);
			sendWeather();
			//	send("cloudMinMax", cloudMinMax);
			send("cloudMinMax", vec2(cube.aabbMin().y, cube.aabbMax().y));
			send("stepSize", stepSize);
			send("camPos", activeCamera().getPosition());
			send("texMin", vec3(cube.aabb().min));
			send("texMax", vec3(cube.aabb().max));
			shade(quad);
			});
	}

	void renderFloor() {
		shader("phong")([&]() {
			send("shadowOn", false);
			send("camPos", activeCamera().getPosition());
			send("lightPos", light[0].position.xyz);
			send("lightColor", vec3(1));
			floor->render();
		});
	}

	void renderClouds() {
		glEnable(GL_BLEND);
		shader("clouds") ([&] {
			send(activeCamera());
			glBindTextureUnit(0, noiseTexture->buffer());
			sendWeather();
		//	send("cloudMinMax", cloudMinMax);
			send("cloudMinMax", vec2(cube.aabbMin().y, cube.aabbMax().y));
			send("stepSize", stepSize);
			send("camPos", activeCamera().getPosition());
			send("texMin", vec3(cube.aabb().min));
			send("texMax", vec3(cube.aabb().max));
			shade(cube);
		});
		glDisable(GL_BLEND);
	}

	void sendWeather() {
		send("weather.cloud_coverage", weather.cloud_coverage);
		send("weather.cloud_type", weather.cloud_type);
		send("weather.percipitation", weather.percipitation);
		send("weather.wind_direciton", weather.wind_direciton);
		send("weather.cloud_speed", weather.cloud_speed);
	}

private:
	ProvidedMesh quad;
	ProvidedMesh noiseQuad;
	ProvidedMesh cubeAABB;
	Compute* noiseGenerator;
	Texture3D* noiseTexture;
	const uvec3 dim{ 512, 512, 128 };
	uvec3 workers = dim / uvec3(8, 8, 8);
	GLuint image = 0;
	Logger logger = Logger::get("Clouds");
	Hemisphere* inner;
	Hemisphere* outer;
	Floor* floor;
	Weather weather;
	Cube cube;
	Sphere sphere;
	const vec2 cloudMinMax = vec2(50, 100);
	const vec3 stepSize = vec3(1) / vec3(10);
	CloudUI* cloudUI;
};