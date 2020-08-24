#pragma once

#include <thread>
#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/common.h"
#include "../GLUtil/include/ncl/gl/FrameBuffer.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "../GLUtil/include/ncl/units/units.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../Physics/Floor.h"
#include "../GLUtil/include/ncl/ray_tracing/RayGenerator.h"
#include "Weather.h"
#include "CloudUI.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;
using namespace unit;

namespace rt = ray_tracing;

const unsigned WIDTH = Resolution::HD.width;
const unsigned HEIGHT = Resolution::HD.width;

class CloudScene : public Scene {
public:
	CloudScene() :Scene{ "Perlin-Worley Clouds", WIDTH, HEIGHT } {
	//	_fullScreen = true;
		camInfoOn = true;
	//	_hideCursor = false;
	//	_requireMouse = true;
		addShader("render", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("phong", GL_VERTEX_SHADER, phong_lfp_vert_shader);
		//		addShader("phong", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("phong", GL_FRAGMENT_SHADER, phong_lfp_frag_shader);
	//	addShader("clouds", GL_VERTEX_SHADER, identity_vert_shader);

		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
	}

	void init() override {
		glDisable(GL_CULL_FACE);
		_modelHeight = 5.0f;
		bounds(vec3(-200), vec3(200));
		initDefaultCamera();
		activeCamera().perspective(60.0f, _width / _height, 10.0_cm, 100.0_km);
	//	activeCamera().collisionTestOff();
	//	activeCamera().setVelocity(vec3(50));
	//	activeCamera().setPosition({ 3.26, 24.6, 13.1 });
	//	deactivateCameraControl();
	//	activeCamera().setPosition({ 0, 0, 60 });
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);

		noiseQuad = ProvidedMesh{ screnSpaceQuad(), false, dim.z };
		noiseQuad.defautMaterial(false);

		// "c:\\temp\\low_frequncy_noise.raw"
		float* noise = nullptr;

		//ifstream fin;
		//fin.open("c:\\temp\\low_frequncy_noise.raw", std::ios_base::binary);

		//if (fin.good()) {
		//	noise = new float[dim.x * dim.y * dim.z * 4];
		//	fin.read(reinterpret_cast<char*>(noise), dim.x * dim.y * dim.z * sizeof(float) * 4);
		//}
		//fin.close();
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		noiseTexture = new Texture3D{
			noise,
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
		auto xform = translate(mat4(1), { 0, 0, 0 });
		xform = scale(xform, vec3(dim));
		//auto xform = mat4(1);
		cube = Cube{ 1, WHITE, vector<mat4>{1, xform }, false };
		cube.defautMaterial(false);
		cubeAABB = aabbOutline(cube.aabb(), BLACK);
	//	cube = Cube{ 1, WHITE};
		sphere = Sphere{ 0.5 };
		cloudUI = new CloudUI{ weather, *this };

		auto config = FrameBuffer::defaultConfig(_width, _height);
		config.attachments[0].internalFmt = GL_RGBA32F;
		config.attachments[0].fmt = GL_RGBA;
		fb = FrameBuffer{ config };

		rayInit();
	}

	void rayInit() {

		initSkyBox();
		camera_ssbo = StorageBufferObj<rt::Camera>{ rt::Camera{} };
		rayGenerator = new rt::RayGenerator{ *this, camera_ssbo };

		ivec3 workers = ivec3{ _width/32, _height / 32, 1 };

		Image2D image = Image2D{ (unsigned)_width, (unsigned)_height, GL_RGBA32F, "scene", 0};

		clouds = new Compute{ workers, { image }, &shader("cloud"), [&] {
			glBindTextureUnit(1, fb.texture());
			glBindTextureUnit(2, noiseTexture->buffer());
			rayGenerator->getRaySSBO().sendToGPU();
			send("atmosphere.innerRadius", float(100));
			send("atmosphere.outerRadius", float(200));
			send("box.min", vec3(0));
			send("box.max", vec3(10));
			send("dt", Timer::get().timeSinceStart());
			send("stepDelta", stepSize);
			sendWeather();

		} };

		addCompute(rayGenerator);
		addCompute(clouds);
	}

	void initSkyBox() {
		vector<string> skyTextures = vector<string>{
			"right.jpg", "left.jpg",
			"top.jpg", "bottom.jpg",
			"front.jpg", "back.jpg"
		};

		string root = "C:\\Users\\" + username + "\\OneDrive\\media\\textures\\skybox\\005\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
			});

		skybox = SkyBox::create(skyTextures, 0, *this);
	}


	void generateNoise() {
		shader("noise")([&] {
			glBindImageTexture(image, noiseTexture->buffer(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glDispatchCompute(workers.x, workers.y, workers.z);
		});

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		float* noise = new float[dim.x * dim.y * dim.z * 4];
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_3D, noiseTexture->buffer());
		glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, (void*)noise);

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		ofstream fout;
		fout.open("c:\\temp\\low_frequncy_noise.raw", std::ios_base::binary);
		if (fout.good()) {
			fout.write(reinterpret_cast<char*>(noise), dim.x * dim.y * dim.z * sizeof(float) * 4);
			fout.flush();
		}
		fout.close();
		delete[] noise;
	}

	void display() override {
	//	cloudUI->render();
		//renderBounds();
	//	renderNoise();
		renderClouds();
	//	renderFloor();
		//shader("flat")([&] {
		//	send(activeCamera());
		//	shade(cubeAABB);
		//	send(activeCamera(), translate(mat4(1), cube.aabbMin()));
		//	shade(sphere);
		//	send(activeCamera(), translate(mat4(1), cube.aabbMax()));
		//	shade(sphere);
		//});


		//shader("screen")([&] {
		//	glBindTextureUnit(0, fb.texture());
		//	shade(quad);
		//});
	}

	void renderNoise() {
		shader("render")([&] {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, noiseTexture->buffer());
		//	glBindTextureUnit(0, noiseTexture->buffer());
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
		//glEnable(GL_BLEND);
		//shader("clouds") ([&] {
		//	send(activeCamera());
		//	glBindTextureUnit(0, noiseTexture->buffer());
		//	sendWeather();
		////	send("cloudMinMax", cloudMinMax);
		//	send("cloudMinMax", vec2(cube.aabbMin().y, cube.aabbMax().y));
		//	send("stepSize", stepSize);
		//	send("camPos", activeCamera().getPosition());
		//	send("texMin", vec3(cube.aabb().min));
		//	send("texMax", vec3(cube.aabb().max));
		//	shade(cube);
		//});
		//glDisable(GL_BLEND);

		//static bool once = true;
		//if (once) {
		//	once = false;
		//	stringstream ss;
		//	rayGenerator->getRaySSBO().read([&](rt::Ray* itr) {
		//		for (int i = 0; i < 10; i++) {
		//			auto ray = *(itr + i);
		//			ss.str("");
		//			ss.clear();
		//			ss << "o: " << ray.origin << ", d: " << ray.direction;
		//			logger.info(ss.str());
		//		}
		//	});
		//}

		shader("screen")([&] {
			clouds->images().front().renderMode();
			glBindTextureUnit(0, clouds->images().front().buffer());
			glBindTextureUnit(0, noiseTexture->buffer());
			shade(quad);
		});
	}

	void sendWeather() {
		send("weather.cloud_coverage", weather.cloud_coverage);
		send("weather.cloud_type", weather.cloud_type);
		send("weather.percipitation", weather.percipitation);
		send("weather.wind_direciton", weather.wind_direciton);
		send("weather.cloud_speed", weather.cloud_speed);

		//logger.info("cloud_coverage" + to_string(weather.cloud_coverage));
	}

	void renderSky() {
		shader("sky")([&] {
			send(activeCamera());
			send("camPos", activeCamera().getPosition());
			send("sunPos", vec3(50.0_km));
			shade(outer);
		});
	}

	void update(float dt) {
		fb.use([&] {
		//	renderSky();
			renderFloor();
		});
	}

private:
	ProvidedMesh quad;
	ProvidedMesh noiseQuad;
	ProvidedMesh cubeAABB;
	Compute* noiseGenerator;
	Texture3D* noiseTexture;
	const uvec3 dim{ 256, 256, 256 };
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
	const vec3 stepSize = vec3(1) / vec3(dim);
	CloudUI* cloudUI;
	rt::RayGenerator* rayGenerator;
	StorageBufferObj<rt::Camera> camera_ssbo;
	SkyBox* skybox;
	Compute* clouds;
	FrameBuffer fb;
};
