#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
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
	LightProbeScene() :Scene("Light probe") {
		addShader("phong", GL_VERTEX_SHADER, scene_capture_vert_shader);
		addShader("phong", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("phong", GL_FRAGMENT_SHADER, scene_capture_phong_frag_shader);

		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);

		addShader("equi_rect", GL_VERTEX_SHADER, equi_rectangular_map_vert_shader);
		addShader("equi_rect", GL_FRAGMENT_SHADER, equi_rectangular_map_frag_shader);

		addShader("irradiance_convolution", GL_VERTEX_SHADER, irradiance_convolution_vert_shader);
		addShader("irradiance_convolution", GL_FRAGMENT_SHADER, irradiance_convolution_frag_shader);
		addShader("prefilter", GL_VERTEX_SHADER, specular_convolution_vert_shader);
		addShader("prefilter", GL_FRAGMENT_SHADER, speculuar_convolution_frag_shader);

		addShader("render", GL_VERTEX_SHADER, probe_render_vert_shader);
		addShader("render", GL_FRAGMENT_SHADER, probe_render_frag_shader);

		addShader("octahedral", GL_VERTEX_SHADER, octahedral_vert_shader);
		addShader("octahedral", GL_GEOMETRY_SHADER, octahedral_geom_shader);
		addShader("octahedral", GL_FRAGMENT_SHADER, octahedral_frag_shader);

		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, octahedral_render_frag_shader);

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

		auto fragmentShader = getText("light_field_probe_input.frag");

		auto config = lightFieldConfig();
		probe = Probe{ this, probePos, 1.0f, config, fragmentShader };

		probes.emplace_back(this, vec3{ 5, 2.43, 5 }, 1.0f, config, fragmentShader);
		probes.emplace_back(this, vec3{ 5, 2.43, -5 }, 1.0f, config, fragmentShader);
		probes.emplace_back(this, vec3{ -5, 2.43, -5 }, 1.0f, config, fragmentShader);
		probes.emplace_back(this, vec3{ -5, 2.43, 5 }, 1.0f, config, fragmentShader);
		probes.emplace_back(this, probePos, 1.0f, config, fragmentShader);

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
		initOctahedral();
	}

	void initOctahedral() {
		quad = ProvidedMesh{ screnSpaceQuad() };
		octahedral = FrameBuffer{ octahedralConfig() };

		vec4 colors[] = { RED, BLUE, YELLOW, GREEN, CYAN };

		int i = 3;
		octahedral.use([&] {
			shader("octahedral")([&] {
				for (int i = 0; i < probes.size(); i++) {
					auto& probe = probes[i];
					send("layer", i);
					send("color", colors[i]);
					glBindTextureUnit(0, probes[i].texture(0));
					glBindTextureUnit(1, probes[i].texture(1));
					glBindTextureUnit(2, probes[i].texture(2));
					shade(quad);
				}
			});
		});
	}

	void display() override {
		//probes[1].render();
		renderOctahedral();

//		shader("phong") ([&] {
//			send(activeCamera());
//			send("camPos", activeCamera().getPosition());
//			send("projection", activeCamera().getProjectionMatrix());
//			send("views[0]", activeCamera().getViewMatrix());
//			renderScene(true);
//		});
//
//		shader("flat")([&] {
// 			auto model = glm::translate(glm::mat4{ 1 }, lightPos);
//			send(activeCamera());
////			shade(lightObj);
//			for (auto& probe : probes) {
//				shade(probe);
//			}
//
//		});
	}

	void renderOctahedral() {
		shader("screen")([&] {
			send("numLayers", int(probes.size()));
			send("layer", 0);
			glBindTextureUnit(0, octahedral.texture());
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

	FrameBuffer::Config lightFieldConfig() {
		auto config = FrameBuffer::Config{ 1024, 1024 };
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
			attachment.numLayers = probes.size();
			config.attachments.push_back(attachment);
		}

		
		return config;
	}

	FrameBuffer::Config octahedralConfig() {
		auto config = FrameBuffer::Config{ 1024, 1024 };
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
			config.attachments.push_back(attachment);
		}

		//config.attachments[1].internalFmt = GL_RG8;
		//config.attachments[2].internalFmt = GL_R16F;

		return config;
	}

	void probeToOctahedral() {

	}

private:
	Logger logger;
	Probe probe;
	vector<Probe> probes;
	FrameBuffer octahedral;
	Model* sponza;
	OminiDirectionalShadowMap shadowMap;
	vec3 lightPos;
	vec3 probePos;
	Sphere* lightObj;
	ProvidedMesh quad;
	float roughness = 0;
};