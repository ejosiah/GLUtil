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

		//addShader("equi_rect", GL_VERTEX_SHADER, equi_rectangular_map_vert_shader);
		//addShader("equi_rect", GL_FRAGMENT_SHADER, equi_rectangular_map_frag_shader);

		//addShader("irradiance_convolution", GL_VERTEX_SHADER, irradiance_convolution_vert_shader);
		//addShader("irradiance_convolution", GL_FRAGMENT_SHADER, irradiance_convolution_frag_shader);
		//addShader("prefilter", GL_VERTEX_SHADER, specular_convolution_vert_shader);
		//addShader("prefilter", GL_FRAGMENT_SHADER, speculuar_convolution_frag_shader);

		addShader("render", GL_VERTEX_SHADER, probe_render_vert_shader);
		addShader("render", GL_FRAGMENT_SHADER, probe_render_frag_shader);

		addShader("octahedral", GL_VERTEX_SHADER, octahedral_vert_shader);
		//addShader("octahedral", GL_GEOMETRY_SHADER, octahedral_geom_shader);
		addShader("octahedral", GL_FRAGMENT_SHADER, octahedral_frag_shader);

		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_GEOMETRY_SHADER, octahedral_render_geom_shader);
		addShader("screen", GL_FRAGMENT_SHADER, octahedral_render_frag_shader);

	//	addShader("prefilter", GL_VERTEX_SHADER, getText("light_probe_prefilter.vert"));
	//	addShader("prefilter", GL_VERTEX_SHADER, screen_vert_shader);
	//	addShader("prefilter", GL_GEOMETRY_SHADER, getText("light_field_probe_prefilter.geom"));
	//	addShader("prefilter", GL_FRAGMENT_SHADER, getText("light_field_probe_prefilter.frag"));

		logger = Logger::get("probe");
		camInfoOn = true;
	}

	void init() override {
		initDefaultCamera();
		activeCamera().collisionTestOff();
		activeCamera().setMode(Camera::FIRST_PERSON);
		activeCamera().setPosition({ 0, 1, 3 });
		sponza = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\Sponza\\sponza.obj", false, 20);
		lightPos = { 0, 2.43, 0.6 };
		shadowMap = OminiDirectionalShadowMap{ 5, lightPos, 2048, 2048 };
		lightObj = new Sphere(0.3, 20, 20, WHITE);
		probePos = { 0, 2.43, 0 };
		min_loc = Sphere{ 1.0, 50, 50, WHITE };
		max_loc = Sphere{ 1.0, 50, 50, WHITE };

		auto center = (sponza->bound->max() + sponza->bound->min()) * 0.5f;

		auto dir = vec3{ 10, 12, 0 } - lightPos;
		X = new Vector{ {1, 0, 0}, center, 1.0, RED };
		Y = new Vector{ {0, 1, 0}, center, 1.0, GREEN };
		Z = new Vector{ {0, 0, 1}, center, 1.0, BLUE };

		auto fragmentShader = getText("light_field_probe_input.frag");

		auto config = probeConfig();
		//probe = Probe{ this, probePos, 1.0f, config, fragmentShader };

		//probes.emplace_back(this, vec3{ 5, 2.43, 5 }, 1.0f, config, fragmentShader);
		//probes.emplace_back(this, vec3{ 5, 2.43, -5 }, 1.0f, config, fragmentShader);
		//probes.emplace_back(this, vec3{ -5, 2.43, -5 }, 1.0f, config, fragmentShader);
		//probes.emplace_back(this, vec3{ -5, 2.43, 5 }, 1.0f, config, fragmentShader);
		//probes.emplace_back(this, probePos, 1.0f, config, fragmentShader);

		initProbes();

		shadowMap.update(lightPos);
		shadowMap.capture([&] {
			sponza->defautMaterial(false);
			shade(sponza);
		});

		//probe.capture([&] {
		//	renderScene();
		//});

		for (auto& probe : probes) {
			probe.capture([&] {
				renderScene();
			});
		}

		cube = Cube{ 1, WHITE, {}, false };
		cube.defautMaterial(false);
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);
		initOctahedral();
		initMeanDistanceProbeGrid();
		initIrradiaceProbeGrid();
	//	skybox = SkyBox{ this, irradianceProbeGrid.texture() };
	}

	void initProbes() {
		auto dim = sponza->bound->max() - sponza->bound->min();
		probeStep = dim / probeCount;
		probeStep.y *= 0.5;
		vec3 probeStart = sponza->bound->min() + probeStep * vec3(0.5, 0.8, 0.5);
		auto fragmentShader = getText("light_field_probe_input.frag");

	//	probeCount = { 2, 2, 2 };
		//probeStep = { 0.1, 0.1, 0.1 };
		//auto probeStart = probePos;

		auto config = probeConfig();
		for (int z = 0; z < probeCount.z; z++) {
			for (int y = 0; y < probeCount.y; y++) {
				for (int x = 0; x < probeCount.x; x++) {
					vec3 location = vec3(x, y, z) * probeStep + probeStart;
					probes.emplace_back(this, location, 1.0f, config, fragmentShader);
				}
			}
		}
	}

	void initOctahedral() {
		
		octahedral = FrameBuffer{ octahedralConfig() };

		vec4 colors[] = { RED, BLUE, YELLOW, GREEN, CYAN };

		octahedral.use([&] {
			shader("octahedral")([&] {
				for (int layer = 0; layer < probes.size(); layer++) {
					octahedral.attachTextureFor(layer);
					auto& probe = probes[layer];
					glBindTextureUnit(0, probes[layer].texture(0));
					glBindTextureUnit(1, probes[layer].texture(1));
					glBindTextureUnit(2, probes[layer].texture(2));
					shade(quad);
				}
			});
		});

	}

	void initIrradiaceProbeGrid() {
		
		
		mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
		glm::mat4 views[6]{
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};

		irradianceProbeGrid = FrameBuffer{ irradianceProbeGridConfig() };
		irradianceProbeGrid.use([&] {
			shader("lfp_prefilter")([&] {
				send("numSamples", 2048);
				send("lobeSize", 1.0f);
				send("irradiance", true);	// TODO try subroutine
				shader("lfp_prefilter").sendUniformMatrix4fv("views", 6, false, value_ptr(views[0]));
				shader("lfp_prefilter").sendUniformMatrix4fv("projection", 1, false, value_ptr(projection));
				for (int layer = 0; layer < probes.size(); layer++) {
					send("layer", layer);
				//	irradianceProbeGrid.attachTextureFor(layer);
					auto& probe = probes[layer];
					glBindTextureUnit(0, probes[layer].texture(0));
					shade(cube);
				}
			});
		});
	}

	void initMeanDistanceProbeGrid() {

		mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
		glm::mat4 views[6]{
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};

		meanDistanceProbeGrid = FrameBuffer{ meanDistanceProbeGridConfig() };
		meanDistanceProbeGrid.use([&] {
			shader("lfp_prefilter")([&] {
				send("numSamples", 128);
				send("lobeSize", 1.0f);
				send("irradiance", false);	// TODO try subroutine
				shader("lfp_prefilter").sendUniformMatrix4fv("views", 6, false, value_ptr(views[0]));
				shader("lfp_prefilter").sendUniformMatrix4fv("projection", 1, false, value_ptr(projection));
				for (int layer = 0; layer < probes.size(); layer++) {
					send("layer", layer);
					//	irradianceProbeGrid.attachTextureFor(layer);
					auto& probe = probes[layer];
					glBindTextureUnit(0, probes[layer].texture(2));
					shade(cube);
				}
				});
			});
	}

	void display() override {
	//	probes[0].render();
	//	renderOctahedral();
	//	renderRealScene();
	//	skybox.render();
		renderIrradiance();
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
			for (auto& probe : probes) {
				shade(probe);
			}

			send("M", translate(mat4{ 1 }, sponza->bound->min()));
			shade(min_loc);
			send("M", translate(mat4{ 1 }, sponza->bound->max()));
			shade(max_loc);

			send("M", mat4{ 1 });
			shade(X);
			shade(Y);
			shade(Z);
		});
	}

	void renderOctahedral() {

		shader("screen")([&] {
			send("isDistance", attachment == 2);
			send("numLayers", int(probes.size()));
			glBindTextureUnit(0, octahedral.texture(attachment));
			glBindTextureUnit(1, irradianceProbeGrid.texture());
			glBindTextureUnit(2, meanDistanceProbeGrid.texture());
			shade(quad);
		});
	}

	void renderScene(bool shadowOn = false) {
		sponza->defautMaterial(true);
		send(shadowMap);
		send("lightPos", lightPos);
		send("shadowOn", shadowOn);
		shade(sponza);
	}

	void renderIrradiance() {
		glDepthFunc(GL_LEQUAL);
		cube.defautMaterial(false);
		shader("lfp_irradiance_render")([&] {
			send("layer", 0);
			//glBindTextureUnit(0, irradianceProbeGrid.texture());
			send("isDistance", true);
			glBindTextureUnit(0, meanDistanceProbeGrid.texture());
			send(activeCamera());
			shade(cube);
		});
		glDepthFunc(GL_LESS);
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

	FrameBuffer::Config probeConfig() {
		auto config = FrameBuffer::Config{ probeRes, probeRes };
		config.fboTarget = GL_FRAMEBUFFER;
		config.depthAndStencil = true;
		config.depthTest = true;
		config.stencilTest = false;

		for (int i = 0; i < 3; i++) {
			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_NEAREST;
			attachment.minfilter = GL_NEAREST;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_CUBE_MAP;
			attachment.internalFmt = GL_RGBA32F;
			attachment.fmt = GL_RGBA;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
			attachment.texLevel = 0;
			config.attachments.push_back(attachment);
		}

		
		return config;
	}

	FrameBuffer::Config octahedralConfig() {
		auto config = FrameBuffer::Config{ probeRes, probeRes };
		config.fboTarget = GL_FRAMEBUFFER;
		config.depthAndStencil = false;
		config.depthTest = false;
		config.stencilTest = false;

		for (int i = 0; i < 3; i++) {
			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_NEAREST;
			attachment.minfilter = GL_NEAREST;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_2D_ARRAY;
			attachment.internalFmt = GL_R11F_G11F_B10F;
			attachment.fmt = GL_RGBA;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
			attachment.texLevel = 0;
			attachment.numLayers = probes.size();
			config.attachments.push_back(attachment);
		}

		config.attachments[2].internalFmt = GL_RG16F;
		config.attachments[2].fmt = GL_RG;

		return config;
	}

	FrameBuffer::Config irradianceProbeGridConfig() {
		auto config = FrameBuffer::Config{ 128, 128 };
		config.fboTarget = GL_FRAMEBUFFER;
		config.depthAndStencil = false;
		config.depthTest = false;
		config.stencilTest = false;

		auto attachment = FrameBuffer::Attachment{};
		attachment.magFilter = GL_NEAREST;
		attachment.minfilter = GL_NEAREST;
		attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
		attachment.texTarget = GL_TEXTURE_CUBE_MAP_ARRAY;
		attachment.internalFmt = GL_R11F_G11F_B10F;
		attachment.fmt = GL_RGBA;
		attachment.type = GL_FLOAT;
		attachment.attachment = GL_COLOR_ATTACHMENT0;
		attachment.texLevel = 0;
		attachment.numLayers = probes.size();
		config.attachments.push_back(attachment);

		return config;
	}
	FrameBuffer::Config meanDistanceProbeGridConfig() {
		auto config = FrameBuffer::Config{ 128, 128 };
		config.fboTarget = GL_FRAMEBUFFER;
		config.depthAndStencil = false;
		config.depthTest = false;
		config.stencilTest = false;

		auto attachment = FrameBuffer::Attachment{};
		attachment.magFilter = GL_NEAREST;
		attachment.minfilter = GL_NEAREST;
		attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
		attachment.texTarget = GL_TEXTURE_CUBE_MAP_ARRAY;
		attachment.internalFmt = GL_RG32F;
		attachment.fmt = GL_RG;
		attachment.type = GL_FLOAT;
		attachment.attachment = GL_COLOR_ATTACHMENT0;
		attachment.texLevel = 0;
		attachment.numLayers = probes.size();
		config.attachments.push_back(attachment);

		return config;
	}

	LightFieldProbes::Config lfpConfig() {
		auto dim = sponza->bound->max() - sponza->bound->min();
		auto config = LightFieldProbes::Config{};
		config.probeCount = { 8, 2, 4 };
		config.probeStep = dim / config.probeCount;
		config.probeStep.y *= 0.5;
		config.startProbeLocation = sponza->bound->min() + probeStep * vec3(0.5, 0.8, 0.5);
		config.captureFragmentShader = getText("light_field_probe_input.frag");
		config.resolution = 1024;
		config.irradiance.numSamples = 2048;
		config.irradiance.lobeSize = 1.0f;
		config.irradiance.resolution = 128;
		config.meanDistance.numSamples = 128;
		config.meanDistance.lobeSize = 1.0f;
		config.meanDistance.resolution = 128;

		return config;
	}

private:
	int attachment = 0;
	Logger logger;
	Probe probe;
	Cube cube;
	GLsizei probeRes = 512;
	vector<Probe> probes;
	FrameBuffer octahedral;
	FrameBuffer irradianceProbeGrid;
	FrameBuffer meanDistanceProbeGrid;
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
	vec3 probeCount{ 8, 2, 4 };
	vec3 probeStep{ 0.2, 0.2, 0.2 };
	Sphere min_loc;
	Sphere max_loc;
	LightFieldSurface lightSurface;
	LightFieldProbes lightFieldProbes;
	float roughness = 0;
};