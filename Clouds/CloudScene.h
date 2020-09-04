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

const unsigned WIDTH = 1920;
const unsigned HEIGHT = 1024;

class CloudScene : public Scene {
public:
	CloudScene() :Scene{ "Perlin-Worley Clouds", WIDTH, HEIGHT } {
	//	_fullScreen = true;
	//	camInfoOn = true;
	//	_hideCursor = false;
	//	_requireMouse = true;
		_fontSize = 15;
		_fontColor = WHITE;
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
		setForeGroundColor(WHITE);
		_modelHeight = 5.0f;
		bounds(vec3(-tMax), vec3(tMax));
		initDefaultCamera();
		activeCamera().perspective(60.0f, float(_width)/_height, 10.0_cm, tMax);
		auto& cam = activeCamera();
	//	activeCamera().perspective(60.0f, _width / _height, 0.1, 1000);
	//	activeCamera().collisionTestOff();
	//	activeCamera().setVelocity(vec3(50));
	//	deactivateCameraControl();
		activeCamera().setAcceleration(vec3(100));
	//	activeCamera().setPosition({ 0, 100, 10.0_km });
	//	activeCamera().setPosition({ 0, 0, 1000 });
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);

		noiseQuad = ProvidedMesh{ screnSpaceQuad(), false, dim.z };
		noiseQuad.defautMaterial(false);

		float* noise = nullptr;

		TextureConfig texConfig;
		texConfig.internalFmt = GL_RGBA32F;
		texConfig.fmt = GL_RGBA;
		texConfig.type = GL_FLOAT;
		texConfig.levels = 5;
		texConfig.mipMap = true;
		Data data{ dim.x, dim.y, dim.z };

		fontColor(BLACK);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//lowFreqNoise = new Texture3D{
		//	noise,
		//	dim.x,
		//	dim.y,
		//	dim.z,
		//	0,
		//	GL_RGBA32F,
		//	GL_RGBA,
		//	glm::vec3{ GL_REPEAT },
		//	glm::vec2{ GL_LINEAR },
		//	GL_FLOAT
		//};

		lowFreqNoise = new Texture3D{ data, texConfig };

		highFreqNoise = new Texture3D{
			noise,
			32,
			32,
			32,
			0,
			GL_RGBA32F,
			GL_RGBA,
			glm::vec3{ GL_REPEAT },
			glm::vec2{ GL_LINEAR },
			GL_FLOAT
		};

		CHECK_GL_ERRORS
		glGenTextures(1, &lowFreqNoise1);
		glBindTexture(GL_TEXTURE_3D, lowFreqNoise1);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, 128, 128, 128);
	//	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 128, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		CHECK_GL_ERRORS
		auto prefix = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\weather\\my3DTextureArray";
		for (auto i = 0; i < 128; i++) {
			auto index = i + 1;
			auto path = to_string(index);
			if (index < 10) path = ".00" + path;
			else if (index < 100) path = ".0" + path;
			else path = "." + path;
			path = prefix + path + ".tga";
			Image image{ path };
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, i, 128, 128, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
			auto error = glGetError();
			logger.info("error loading noise image: " + to_string(error));
			CHECK_GL_ERRORS
		}
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		CHECK_GL_ERRORS
		glBindTexture(GL_TEXTURE_3D, 0);
		

		weatherData = new Texture2D{ "C:\\Users\\Josiah\\OneDrive\\media\\textures\\weather\\weather04.png"};

		int val;
		glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &val);
		logger.info("Max GL invocations: " + to_string(val));

		generateNoise();
		
		setBackGroundColor({0.70, 0.76, 0.83, 1.0});
		auto lightPos = vec3{ 0, 500, 10 };
		light[0].position = vec4{ lightPos, 1 };
		floor = new Floor(this, vec2(l));
		
		inner = new Hemisphere{ r0, 20, 20, RED };
		outer = new Hemisphere{ r1, 20, 20, GREEN };
		auto xform = translate(mat4(1), { 0, 50, 0 });
		xform = scale(xform, vec3(100));
	//	auto xform = mat4(1);
		cube = Cube{ 1, WHITE, vector<mat4>{1, xform }, false };
		cube.defautMaterial(false);
		auto aabb = cube.aabb();
		cubeAABB = aabbOutline(cube.aabb(), BLACK);
	//	cube = Cube{ 1, WHITE};
		sphere = Sphere{ 0.5 };
		weather.cloud_coverage = 0.70;
		weather.cloud_type = 0.8;
		weather.percipitation = 1.0;
		cloudUI = new CloudUI{ weather, *this };

		auto config = FrameBuffer::defaultConfig(_width, _height);
		config.attachments[0].internalFmt = GL_RGBA32F;
		config.attachments[0].fmt = GL_RGBA;

		FrameBuffer::Attachment depthAttach;
		depthAttach.attachment = GL_DEPTH_ATTACHMENT;
		depthAttach.magFilter = depthAttach.minfilter = GL_NEAREST;
		depthAttach.wrap_s = depthAttach.wrap_t = depthAttach.wrap_r = GL_CLAMP_TO_EDGE;
		depthAttach.internalFmt = depthAttach.fmt = GL_DEPTH_COMPONENT;
		depthAttach.type = GL_FLOAT;
		depthAttach.attachment = GL_DEPTH_ATTACHMENT;
		config.attachments.push_back(depthAttach);
		config.clearColor = vec4(0.53, 0.81, 0.92, 1.0);
		config.depthTest = true;
		config.stencilTest = false;
		config.depthAndStencil = false;
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
			glBindTextureUnit(2, fb.texture(1));
			glBindTextureUnit(3, lowFreqNoise->buffer());
		//	glBindTextureUnit(3, lowFreqNoise1);
			glBindTextureUnit(4, highFreqNoise->buffer());
			glBindTextureUnit(5, weatherData->bufferId());
			rayGenerator->getRaySSBO().sendToGPU();
			send(activeCamera());
			send("atmosphere.innerRadius", float(100));
			send("atmosphere.outerRadius", float(200));
			send("dt", Timer::get().timeSinceStart());
			send("cloudMinMax", vec2(cube.aabbMin().y, cube.aabbMax().y));
			send("stepSize", stepSize);
			send("camPos", activeCamera().getPosition());
			send("bMin", cube.aabbMin());
			send("bMax", cube.aabbMax());
			send("dt", Timer::get().timeSinceStart());
			send("lightPos", vec3(50.0_km));
			send("tMax", tMax);
			//send("r0", r0);
			//send("r1", r1);
			//send("center", vec3(0, -center, 0));
			send("r0", r0);
			send("r1", r1);
			send("center", vec3(0, -center, 0));
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

			for (int level = 0; level < 5; level++) {
				unsigned factor = std::pow(2.0, level);
				auto numGroups = workers / factor;
				send("octave", 0);
				send("doPerlinWorley", true);
				glBindImageTexture(0, lowFreqNoise->buffer(), level, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
				glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
			}

			send("octave", 3);
			send("doPerlinWorley", false);
			glBindImageTexture(0, highFreqNoise->buffer(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glDispatchCompute(4, 4, 4);
		});
	}

	void display() override {
	//	cloudUI->render();
		//renderBounds();
	//	renderNoise();
	//	renderSky();
	//	renderFloor();

		renderClouds();
	//	shader("flat")([&] {

			//shade(cubeAABB);
			//send(activeCamera(), translate(mat4(1), cube.aabbMin()));
		//	shade(sphere);
		//	send(activeCamera(), translate(mat4(1), cube.aabbMax()));
		//	shade(sphere);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//	auto model = translate(mat4(1), { 0, -center, 0 });
		//	send(activeCamera(), model);
		//	shade(inner);
		//	shade(outer);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//});


		//shader("screen")([&] {
		//	glBindTextureUnit(0, fb.texture());
		//	shade(quad);
		//});

		sbr.str("");
		sbr.clear();
		sbr << "Weather Data:\n";
		sbr << "\tcloud coverage:\t" << weather.cloud_coverage << "\n";
		sbr << "\tcloud type:\t\t" << weather.cloud_type << "\n";
		sbr << "\tprecipitation:\t\t" << weather.percipitation << "\n";
		sFont->render(sbr.str(), 20, 20);
		if (showRay) {
			

			float t0, t1;
			bool aHit = false;
			if (intersectShell(ray, r0, r1, vec3(0, 0, 0), t0, t1)) {
				aHit = true;
				//color = vec4(1, 0, 0, 1);
			}
			auto p = ray.origin + ray.direction * t0;
			auto d = depthValue(p);
			sbr << "origin: " << ray.origin << ", direction: " << ray.direction;
			sbr << "\nposition: " << p;
			sbr << "\ndepth value: " << d;
			sFont->render(sbr.str(), 20, 100);
		}
	}

	void renderNoise() {
		shader("render")([&] {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, lowFreqNoise->buffer());
		//	glBindTexture(GL_TEXTURE_3D, lowFreqNoise1);
		//	glBindTextureUnit(0, lowFreqNoise->buffer());
			send("dt", Timer::get().timeSinceStart());
			send("numSlices", int(dim.z));
			send("slice", float(slice));
			sendWeather();
			//	send("cloudMinMax", cloudMinMax);	
			send("cloudMinMax", vec2(cube.aabbMin().x, cube.aabbMax().y));
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
	stringstream ss;
	void renderClouds() {
		//shader("ray_marching") ([&] {
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//	glBindTextureUnit(0, lowFreqNoise->buffer());
		//	glBindTextureUnit(1, highFreqNoise->buffer());
		//	send(activeCamera());
		//	sendWeather();
		//	send("cloudMinMax", vec2(cube.aabbMin().y, cube.aabbMax().y));
		//	send("stepSize", stepSize);
		//	send("camPos", activeCamera().getPosition());
		//	send("bMin", cube.aabbMin());
		//	send("bMax", cube.aabbMax());
		//	send("dt", Timer::get().timeSinceStart());
		//	send("lightPos", vec3(50.0_km));
		//	shade(cube);
		//	glDisable(GL_BLEND);
		//});



		shader("screen")([&] {
			clouds->images().front().renderMode();
			glBindTextureUnit(0, clouds->images().front().buffer());
			glBindTextureUnit(1, lowFreqNoise1);
			shade(quad);
		});

		if (vRay) {
			glDepthFunc(GL_ALWAYS);
			shader("flat")([&] {
				send(activeCamera());
				shade(vRay);
			});
			glDepthFunc(GL_LESS);
		}
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
	//	setBackGroundColor({ 0.5, 0.5, 1, 1 });
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case ' ':
				slice += 1;
				slice %= dim.z;
				break;
			case 'y':
				weather.cloud_coverage += 0.01;
				break;
			case 'Y':
				weather.cloud_coverage -= 0.01;
				break;
			case 'u':
				weather.cloud_type += 0.01;
				break;
			case 'U':
				weather.cloud_type -= 0.01;
				break;
			case 'i':
				weather.percipitation += 0.01;
				break;
			case 'I':
				weather.percipitation -= 0.01;
				break;
			case 'l':
				if (vRay) delete vRay;
				rayGenerator->getRaySSBO().read([&](rt::Ray* itr) { ray = *itr; });
				vRay = new Vector{ ray.direction, ray.origin, 10, RED };
				showRay = true;
				break;
			}
			weather.cloud_coverage = glm::clamp(weather.cloud_coverage, 0.1f, 0.9f);
			weather.cloud_type = glm::clamp(weather.cloud_type, 0.0f, 1.0f);
			weather.percipitation = glm::clamp(weather.percipitation, 0.0f, 1.0f);
		}
	}

	float depthValue(vec3 p) {
		auto MVP = activeCamera().getViewProjectionMatrix();
		vec4 pos = MVP * vec4(p, 1);

		if (pos.w == 0) return 1;

		// apply perspective division
		float d = pos.z / pos.w;

		// outside NDC [-1, 1]
		if (d < -1 || d > 1) return 1;

		// remap from NDC [-1, 1] to screen space [0, 1]
		return d * 0.5 + 0.5;
	}

	bool intersectSphere(rt::Ray ray, float radius, vec3 center, float& t) {
		vec3 cs = center;
		vec3 m = ray.origin - cs;
		float b = dot(m, ray.direction);
		float c = dot(m, m) - radius * radius;

		// ray is facing away from sphere
		if (c > 0 && b > 0) return false;

		float discr = b * b - c;

		// sqrt(-discr) imaginary number corresponds to ray missing sphere
		if (discr < 0) return false;

		//t = 0;
		t = -b - sqrt(discr);

		return true;
	}


	bool intersectShell(rt::Ray ray, float r0, float r1, vec3 center, float& t0, float& t1) {

		float tInner;
		bool inner = intersectSphere(ray, r0, center, tInner);

		float tOuter;
		bool outer = intersectSphere(ray, r1, center, tOuter);

		t0 = std::min(tOuter, tInner);
		t1 = std::max(tOuter, tInner);

		return inner && outer;
	}

private:
	ProvidedMesh quad;
	ProvidedMesh noiseQuad;
	ProvidedMesh cubeAABB;
	Compute* noiseGenerator;
	Texture3D* lowFreqNoise;
	GLuint lowFreqNoise1;
	Texture3D* highFreqNoise;
	Texture2D* weatherData;
	const uvec3 dim = uvec3(128);
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
	const float h = 1.0_km;
	const float l = 5.0_km;
	const float diag = std::sqrt(l * l + l * l);
	const float r0 = (std::pow(diag / 2, 2) + std::pow(h, 2)) / (2 * h);
	const float r1 = r0 + 0.5_km;
	const float center = r0 - h;
	Vector* vRay = nullptr;
	rt::Ray ray;
	bool showRay = false;
	int slice = 0;
	float tMax = 100.0_km;
};