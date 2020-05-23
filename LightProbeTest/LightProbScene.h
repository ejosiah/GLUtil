#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/light_field_probes.h"
#include "../GLUtil/include/ncl/gl/LightProbe.h"
#include "../GLUtil/include/ncl/gl/ShadowMap.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "../GLUtil/include/ncl/util/SphericalCoord.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class LightProbeScene : public Scene {
public:
	LightProbeScene() :Scene("Light probe", 1028, 1028) {
		addShader("phong", GL_VERTEX_SHADER, scene_capture_vert_shader);
		addShader("phong", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("phong", GL_FRAGMENT_SHADER, scene_capture_phong_frag_shader);

		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);

		addShader("render", GL_VERTEX_SHADER, probe_render_vert_shader);
		addShader("render", GL_FRAGMENT_SHADER, probe_render_frag_shader);

		addShader("octahedral", GL_VERTEX_SHADER, octahedral_vert_shader);
		//addShader("octahedral", GL_GEOMETRY_SHADER, octahedral_geom_shader);
		addShader("octahedral", GL_FRAGMENT_SHADER, octahedral_frag_shader);

		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_GEOMETRY_SHADER, octahedral_render_geom_shader);
		addShader("screen", GL_FRAGMENT_SHADER, octahedral_render_frag_shader);

		addShader("lfp_test", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("lfp_test", GL_FRAGMENT_SHADER, getText("lfp_uniform_test.frag"));

		logger = Logger::get("probe");
		camInfoOn = true;
	}

	void init() override {
		initDefaultCamera();
		activeCamera().collisionTestOff();
		activeCamera().setMode(Camera::FIRST_PERSON);
		activeCamera().setPosition({ 0, 1, 3 });
		setForeGroundColor(WHITE);
		sponza = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\Sponza\\sponza.obj", false, 20);
		lightPos = { 0, 2.43, 0.6 };
		shadowMap = OminiDirectionalShadowMap{ 5, lightPos, 2048, 2048 };
		lightObj = new Sphere(0.3, 20, 20, WHITE);
		probePos = { 0, 2.43, 0 };

		auto center = (sponza->bound->max() + sponza->bound->min()) * 0.5f;

		auto dir = vec3{ 10, 12, 0 } - lightPos;
		X = new Vector{ {1, 0, 0}, center, 1.0, RED };
		Y = new Vector{ {0, 1, 0}, center, 1.0, GREEN };
		Z = new Vector{ {0, 0, 1}, center, 1.0, BLUE };

		shadowMap.update(lightPos);
		shadowMap.capture([&] {
			sponza->defautMaterial(false);
			shade(sponza);
		});

		//for (auto& probe : probes) {
		//	probe.capture([&] {
		//		renderScene();
		//	});
		//}

		//cube = Cube{ 1, WHITE, {}, false };
		//cube.defautMaterial(false);
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);
		//initOctahedral();
		//initMeanDistanceProbeGrid();
		//initIrradiaceProbeGrid();
	//	skybox = SkyBox{ this, irradianceProbeGrid.texture() };
		lightFieldProbes = LightFieldProbes{ lfpConfig(), this };
		lightFieldProbes.init();
		lightFieldProbes.capture([&] {
			renderScene();
		});

		LightFieldSurface c = lightSurface;
		LightFieldSurface a;
		LightFieldSurface b;
	}



	void display() override {
	//	probes[0].render();
	//	renderOctahedral();
	//	renderRealScene();
	//	skybox.render();
	//	renderIrradiance();
	//	lightFieldProbes.renderProbe(28);
	//	lightFieldProbes.renderOctahedrals();
	//	lightFieldProbes.renderLowResDistanceProbe();
	//	lightFieldProbes.renderIrradiance();
	//	lightFieldProbes.renderMeanDistance();
		shader("lfp_test")([&] {
			send("layer", 21);
			lightFieldProbes.sendTo(shader("lfp_test"));
			shade(quad);
		});
	}

	void renderRealScene() {
		shader("phong") ([&] {
			send(activeCamera());
			send("camPos", activeCamera().getPosition());
			send("projection", activeCamera().getProjectionMatrix());
			send("views[0]", activeCamera().getViewMatrix());
			renderScene(true);
		});

		shader("flat")([&] {
 			auto model = glm::translate(glm::mat4{ 1 }, lightPos);
			send(activeCamera(), model);
			shade(lightObj);
			shade(lightFieldProbes);

			send("M", mat4{ 1 });
			shade(X);
			shade(Y);
			shade(Z);
		});
	}

	void renderScene(bool shadowOn = false) {
		sponza->defautMaterial(true);
		send(shadowMap);
		send("lightPos", lightPos);
		send("shadowOn", shadowOn);
		shade(sponza);
	}

	void update(float dt) override{
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			if (key.value() == '=') {
				roughness += 0.05;
			}
			if (key.value() == '-') {
				roughness -= 0.05;
			}
			roughness = glm::clamp(roughness, 0.0f, 1.0f);
		}
	}


	LightFieldProbes::Config lfpConfig() {
		auto dim = sponza->bound->max() - sponza->bound->min();
		auto config = LightFieldProbes::Config{};
		config.probeCount = { 8, 2, 4 };
		config.probeStep = dim / config.probeCount;
		config.probeStep.y *= 0.5;
		config.startProbeLocation = sponza->bound->min() + config.probeStep * vec3(0.5, 0.8, 0.5);
		config.captureFragmentShader = getText("light_field_probe_input.frag");
		config.resolution = 512;
		config.irradiance.numSamples = 2048;
		config.irradiance.lobeSize = 1.0f;
		config.irradiance.resolution = 128;
		config.meanDistance.numSamples = 128;
		config.meanDistance.lobeSize = 1.0f;
		config.meanDistance.resolution = 128;
		config.lowResolutionDownsampleFactor = 8;

		return config;
	}

private:
	int attachment = 0;
	Logger logger;
	Probe probe;
	Cube cube;
	GLsizei probeRes = 512;
	Model* sponza;
	OminiDirectionalShadowMap shadowMap;
	vec3 lightPos;
	vec3 probePos;
	Sphere* lightObj;
	ProvidedMesh quad;
	SkyBox skybox;
	Vector* X;
	Vector* Y;
	Vector* Z;
	LightFieldSurface lightSurface;
	LightFieldProbes lightFieldProbes;
	float roughness = 0;
};