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
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
		//addShader("screen", GL_GEOMETRY_SHADER, octahedral_render_geom_shader);
		//addShader("screen", GL_FRAGMENT_SHADER, octahedral_render_frag_shader);

		//addShader("lfp_test", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("lfp_test", GL_VERTEX_SHADER, scene_capture_vert_shader);
		addShader("lfp_test", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("lfp_test", GL_FRAGMENT_SHADER, getText("lfp_uniform_test.frag"));

		addShader("mirror", GL_VERTEX_SHADER, scene_capture_vert_shader);
		addShader("mirror", GL_GEOMETRY_SHADER, scene_capture_geom_shader);

		logger = Logger::get("probe");
		camInfoOn = false;
	}

	void init() override {
		initDefaultCamera();
		activeCamera().collisionTestOff();
		activeCamera().setMode(Camera::FIRST_PERSON);
		activeCamera().setPosition({ 0, 1, 3 });
		setForeGroundColor(WHITE);
		//sponza = new Model("C:\\Users\\Josiah\\source\\repos\\Precomputed-Light-Field-Probes\\assets\\sponza\\sponza.obj", false, 20);
		sponza = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\Sponza\\sponza.obj", false, 20);
		lightPos = { 0, 5, 0.6 };
		shadowMap = OminiDirectionalShadowMap{ 5, lightPos, 2048, 2048 };
		lightColor = vec3(0.3);
		lightObj = new Sphere(0.3, 20, 20, vec4(lightColor, 1));
		probePos = { 0, 2.43, 0 };

		center = (sponza->bound->max() + sponza->bound->min()) * 0.5f;

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

		cube = Cube{ 1, WHITE, {}, false };
		cube.defautMaterial(false);
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);
		//initOctahedral();
		//initMeanDistanceProbeGrid();
		//initIrradiaceProbeGrid();
	//	skybox = SkyBox{ this, irradianceProbeGrid.texture() };
		auto config = lfpConfig();
		//auto mem = memoryUse(config);
		//logger.info("lfp memory use: " + to_string(mem) + " MB");

		GLint data;
		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &data);
		logger.info("max fb width: " + to_string(data));
		glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &data);
		logger.info("max layers width: " + to_string(data));

		lightFieldProbes = LightFieldProbes{ config, this };
		lightFieldProbes.init();
		lightFieldProbes.capture([&] {
			renderScene();
		});

		LightFieldSurface c = lightSurface;
		LightFieldSurface a;
		LightFieldSurface b;
		clearBindings();
		createMirror();

		convolution = FrameBuffer{ cConfig() };
		convolutionShader.load({ GL_VERTEX_SHADER, octahedral_vert_shader, "octahedral_convolution.vert" });
		convolutionShader.load({ GL_FRAGMENT_SHADER, octahedral_convolution_frag_shader, "octahedral_convolution.frag" });
		convolutionShader.createAndLinkProgram();

		convolution.use([&] {
			convolutionShader([&] {
				for (int layer = 0; layer < 1; layer++) {
					const int lod = 6;
					for (int level = 0; level < lod; level++) {
						unsigned int w = 512 * std::pow(0.5, level);
						unsigned int h = 512 * std::pow(0.5, level);
						convolution.attachTextureFor(layer, level);
						glViewport(0, 0, w, h);

						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glBindTextureUnit(0, lightFieldProbes.octahedral.texture(toInt(Lfp::Radiance)));
						float roughness = (float)level / (float)(lod - 1);
						convolutionShader.sendUniform1i("layer", layer);
						convolutionShader.sendUniform1f("roughness", roughness);
						convolutionShader.sendUniform1f("resolution", 512);
						quad.draw(convolutionShader);
					}
				}
			});
		});
	}

	void createMirror() {
		Mesh mesh;

		mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
		mesh.positions.emplace_back(-1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, 1.0f, 0.0f);

		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);
		mesh.normals.emplace_back(0, 0, 1);

		mesh.tangents.emplace_back(1, 0, 0);
		mesh.tangents.emplace_back(1, 0, 0);
		mesh.tangents.emplace_back(1, 0, 0);
		mesh.tangents.emplace_back(1, 0, 0);
		mesh.tangents.emplace_back(1, 0, 0);
		mesh.tangents.emplace_back(1, 0, 0);

		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.bitangents.emplace_back(0, 1, 0);
		mesh.primitiveType = GL_TRIANGLES;

		mirror = ProvidedMesh(mesh);
	}

	void display() override {
	//	probes[0].render();
	//	renderOctahedral();
	//	renderRealScene();
	//	skybox.render();
	//	renderIrradiance();
		lightFieldProbes.renderProbe(28);
	//	lightFieldProbes.renderOctahedrals();
	//	lightFieldProbes.renderLowResDistanceProbe();
	//	lightFieldProbes.renderIrradiance();
	//	lightFieldProbes.renderMeanDistance();
		//shader("lfp_test")([&] {
		//	send(activeCamera());
		//	send("camPos", activeCamera().getPosition());
		//	send("projection", activeCamera().getProjectionMatrix());
		//	send("views[0]", activeCamera().getViewMatrix());
		//	send("lfp_on", lfp_on);
		//	send("layer", 0);
		//	lightFieldProbes.sendTo(shader("lfp_test"));
		//	//shade(quad);
		//	shade(cube);
		//});

		//shader("screen")([&] {
		//	glBindTextureUnit(0, convolution.texture());
		////	glBindTextureUnit(1, lightFieldProbes.octahedral.texture());
		//	shade(quad);
		//});
	}

	void renderRealScene() {
		shader("phong") ([&] {
			send(activeCamera());
			send("camPos", activeCamera().getPosition());
			send("projection", activeCamera().getProjectionMatrix());
			send("views[0]", activeCamera().getViewMatrix());
			send("lfp_on", lfp_on);
			lightFieldProbes.sendTo(shader("phong"));
			renderScene(true);
		});

		//shader("flat")([&] {
 	//		auto model = glm::translate(glm::mat4{ 1 }, lightPos);
		//	send(activeCamera(), model);
		//	shade(lightObj);
		//	shade(lightFieldProbes);

		//	send("M", mat4{ 1 });
		//	shade(X);
		//	shade(Y);
		//	shade(Z);
		//});

		//shader("mirror")([&] {
		//	send(activeCamera());
		//	send("camPos", activeCamera().getPosition());
		//	send("projection", activeCamera().getProjectionMatrix());
		//	send("views[0]", activeCamera().getViewMatrix());
		//	send("lfp_on", lfp_on);
		//	lightFieldProbes.sendTo(shader("mirror"));
		//	send("M", translate(mat4(1), center) * scale(mat4(1), vec3(1.5)));
		//	shade(mirror);
		//});

		//if (lfp_on) {
		//	sFont->render("light Field Probe on", 0, 60);
		//}
		//else {
		//	sFont->render("light Field Probe off", 0, 60);
		//}
		//sFont->render("meshId: " + to_string(meshId), 0, 70);
	}

	void renderScene(bool shadowOn = false, bool all = true) {
		sponza->defautMaterial(true);
		send(shadowMap);
		send("lightColor", lightColor);
		send("lightPos", lightPos);
		send("shadowOn", shadowOn);

		if (all) {
			shade(sponza);
		}
		else {
			sponza->draw(*Shader::boundShader, meshId);
		}

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

			if (key.value() == 'o') {
				lfp_on = !lfp_on;
			}
			if (key.value() == '=') {
				meshId += 1;
			}
			else if (key.value() == '-') {
				meshId -= 0;
			}
			meshId = glm::clamp(meshId, 0, sponza->numMeshes() - 1);
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
		//config.octResolution = 1024;
		config.irradiance.numSamples = 2048;
		config.irradiance.lobeSize = 1.0f;
		config.irradiance.resolution = 128;
		config.meanDistance.numSamples = 128;
		config.meanDistance.lobeSize = 1.0f;
		config.meanDistance.resolution = 128;
		config.lowResolutionDownsampleFactor = 8;
		config.textureBindOffset = 6;

		return config;
	}

	FrameBuffer::Config cConfig() {
		auto config = FrameBuffer::Config{ 512, 512 };
		config.fboTarget = GL_FRAMEBUFFER;
		config.depthAndStencil = false;
		config.depthTest = false;
		config.stencilTest = false;
		auto attachment = FrameBuffer::Attachment{};
		attachment.magFilter = GL_NEAREST;
		attachment.minfilter = GL_NEAREST;
		attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
		attachment.texTarget = GL_TEXTURE_2D;
	//	attachment.texTarget = GL_TEXTURE_2D_ARRAY;
		attachment.internalFmt = GL_R11F_G11F_B10F;
		attachment.fmt = GL_RGB;
		attachment.type = GL_FLOAT;
		attachment.attachment = GL_COLOR_ATTACHMENT0;
	//	attachment.numLayers = 64;
		attachment.numLayers = 1;
		attachment.mipMap = true;
		attachment.texLevel = 0;
		config.attachments.push_back(attachment);

		return config;
	}

	//double memoryUse(LightFieldProbes::Config& config) {
	//	auto numProbes = config.probeCount.x * config.probeCount.y * config.probeCount.z;
	//	double total = config.resolution * config.resolution * numProbes * 6 * 32/8 * 4;
	//	total += config.octResolution * config.octResolution * numProbes * 2 * ;
	//	total += config.octResolution * config.octResolution * numProbes * 16/8 * 2;
	//	total += std::pow(config.octResolution * (1 / float(config.lowResolutionDownsampleFactor)), 2) * numProbes;
	//	total += std::pow(config.irradiance.resolution, 2) * numProbes * 6;
	//	total += std::pow(config.meanDistance.resolution, 2) * numProbes * 6;

	//	return total / std::pow(1024, 2);
	//}

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
	ProvidedMesh mirror;
	LightFieldProbes lightFieldProbes;
	FrameBuffer convolution;
	Shader convolutionShader;
	bool lfp_on = false;
	float roughness = 0;
	int meshId = 0;
	vec3 lightColor;
	vec3 center;
};