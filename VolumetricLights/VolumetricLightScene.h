#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include "../GLUtil/include/ncl/sampling/sampling.h"
#include "../GLUtil/include/ncl/gl/ShadowMap.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../GLUtil/include/ncl/ray_tracing/RayGenerator.h"
#include "../GLUtil/include/ncl/physics/particle/render_particles.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

namespace rt = ray_tracing;
namespace pm = physics::pm;

class VolumetricLightScene : public Scene {
public:
	VolumetricLightScene() :Scene("Volumetric Lights", 2048, 1024) {
		addShader("debug_shadow_map", GL_VERTEX_SHADER, point_shadow_map_render_vert_shader);
		addShader("debug_shadow_map", GL_FRAGMENT_SHADER, point_shadow_map_render_frag_shader);
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		camInfoOn = true;
	}

	void init() override {
		fontColor(WHITE);
		fontSize(15);
		initDefaultCamera();
		activeCamera().setPosition({ 0, 0, 10 });
	//	activeCamera().setPosition({ -1.64, 0.736, -4.67 });
		activeCamera().collisionTestOff();
		sphere = Sphere{ 0.1 };
		sphere.defautMaterial(false);
		sphere.cullBackFaceOff();
		sphere0 = Sphere{ 0.98, 10, 10, vec4(lightColor,1 ) };
		sphere0.defautMaterial(false);
		sphere0.cullBackFaceOff();

		sphereBounds = Sphere{ 3 };
		sphereBounds.defautMaterial(false);
		sphereBounds.cullBackFaceOff();

		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);

		cam.view = lookAt({ 0, 0, 5 }, vec3(0), { 0, 1, 0 });

		auto points = sampling::hammersleySphere(numPoints);

		vector<vec4> holes;
		Mesh mesh;
		mesh.primitiveType = GL_POINTS;

		for (auto p : points) {
			auto num_p = normalize(p);
			holes.push_back(vec4(p, 0.2));
			mesh.positions.push_back(p);
			mesh.colors.push_back(GREEN);
		}
		holesSsbo = StorageBuffer<vec4>{ holes, 3 };
		pMesh = ProvidedMesh{ mesh };
		vec3 p = sampling::pointInSphere(0.5, 0.8);
		float r = length(p);
		logger.info("radius: " + to_string(r));
	//	glPointSize(5);

		auto onFragment = getSource("holes_onFragment.frag")->data;

	//	CHECK_GL_ERRORS
		shadowMap = OminiDirectionalShadowMap{ 5, lightPos, 1024, 1024, 0.1f, farPlane, onFragment};
	//	CHECK_GL_ERRORS
		cube = Cube{ 1 };

		particles = pm::Particles(holes, sphere);
	//	particles = pm::Particles(holes);

		initFrameBuffer();
		initVolumeLightCompute();

		setBackGroundColor(BLACK);
	}

	void initFrameBuffer() {
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
		config.clearColor = vec4(0);
		config.depthTest = true;
		config.stencilTest = false;
		config.depthAndStencil = false;
		fb = FrameBuffer{ config };
	}

	void initVolumeLightCompute() {
		camera_ssbo = StorageBuffer<rt::Camera>{ rt::Camera{} };
		rayGenerator = new rt::RayGenerator{ *this, camera_ssbo };

		ivec3 workers = ivec3{ _width / 32, _height / 32, 1 };
		Image2D colorBuffer = Image2D{ (unsigned)_width, (unsigned)_height, GL_RGBA32F, "out_color_buffer", 0 };

		Image2D depthBuffer = Image2D{ (unsigned)_width, (unsigned)_height, GL_RGBA32F, "out_depth_buffer", 1 };

		volumeLight = new Compute{ workers, {colorBuffer, depthBuffer}, &shader("volume_light"), [&] {
			glBindTextureUnit(2, fb.texture());
			glBindTextureUnit(3, fb.texture(1));
			glBindTextureUnit(4, shadowMap.texture());
			rayGenerator->getRaySSBO().sendToGPU();
			send(activeCamera());
			send("light_pos", lightPos);
			send("cam_pos", activeCamera().getPosition());
			send("volume_radius", 5.0f);
			send("light_radius", 1.0f);
			send("far_plane", farPlane);
			send("g", g);
			send("light_color", lightColor);
		} };

		addCompute(rayGenerator);
		addCompute(volumeLight);
	}

	void initParticles() {
		positionBuffer = TextureBuffer("particle_position", nullptr, sizeof(vec4) * numParticles);
		velocityBuffer = TextureBuffer("particle_velocity", nullptr, sizeof(vec4) * numParticles);
	}

	void display() override {
		//shadowMap.update(lightPos);
		//shadowMap.capture([&] {
		//	holesSsbo.sendToGPU();
		//	send("num_holes", numPoints);
		//	shade(sphere);
		//});
		//

		//shader("holes")([&] {
		//	holesSsbo.sendToGPU();
		//	send("num_holes", numPoints);
		//	send("light_color", lightColor);
		//	send(activeCamera());
		//	shade(sphere);
		//});
		//shader("flat")([&] {
		//	send(activeCamera());
		//	shade(sphere0);
		//});
		//
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//shader("screen")([&] {
		//	volumeLight->images().front().renderMode();
		//	glBindTextureUnit(0, volumeLight->images().at(0).buffer());
		//	glBindTextureUnit(1, volumeLight->images().at(1).buffer());
		//	shade(quad);
		//});
		//glDisable(GL_BLEND);

		//shader("debug")([&] {
		//	glBindTextureUnit(0, shadowMap.texture());
		//	send(activeCamera());
		//	send("light_pos", lightPos);
		//	shade(sphereBounds);
		//});

		shader("particle")([&] {
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			send(activeCamera());
			/*subroutineGeom("shape", "Torus");*/
			//particles.swapBuffers();
			shade(particles);
		//	shade(sphere);
		});

	}

	void resized() override {
		cam.projection = perspective(radians(60.0f), aspectRatio, 0.1f, 100.0f);
	}

	void update() {
		//fb.use([&] {
		//	shader("flat")([&] {
		//		send(activeCamera());
		//		shade(sphere0);
		//		});
		//	shader("holes")([&] {
		//		holesSsbo.sendToGPU();
		//		send("num_holes", numPoints);
		//		send(activeCamera());
		//		shade(sphere);
		//		});
		//});
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case '-':
				g -= 0.01;
				break;
			case '=':
				g += 0.01;
				break;
			}
			g = glm::clamp(g, -1.0f, 1.0f);
		}
	}

private:
	Model* sponza;
	int numPoints = 20;
	Sphere sphere;
	Sphere sphere0;
	Sphere sphereBounds;
	StorageBuffer<vec4> holesSsbo;
	pm::Particles particles;
	ProvidedMesh pMesh;
	Logger logger = Logger::get("VL");
	OminiDirectionalShadowMap shadowMap;
	vec3 lightPos = vec3(0);
	rt::RayGenerator* rayGenerator;
	StorageBuffer<rt::Camera> camera_ssbo;
	Compute* volumeLight;
	vec3 lightColor = { 0.9, 0.8, 0.4 };
	FrameBuffer fb;
	Cube cube;
	float g = 0.2;
	float farPlane = 1000;
	ProvidedMesh quad;
	TextureBuffer positionBuffer;
	TextureBuffer velocityBuffer;
	const int numParticles = 1000000;
};