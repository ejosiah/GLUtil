#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/ShadowMap.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "Cannon.h"
#include "CannonBall.h"
#include "CannonBallReset.h"
#include "Floor.h"
#include "../GLUtil/include/ncl/physics/particle/Particle.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleRegistry.h"
#include "../GLUtil/include/ncl/physics/particle/ParticleIntegrator.h"
#include "../GLUtil/include/ncl/physics/particle/ForceGenerator.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace physics::pm;
using namespace glm;

static int w = Resolution::QHD.width;
static int h = Resolution::QHD.height;

class PhysicsScene : public Scene {
public:
	PhysicsScene(const char* title) :Scene(title, w, h) {
		camInfoOn = true;
		_vsync = true;
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);
		addShader("phong", GL_VERTEX_SHADER, phong_lfp_vert_shader);
//		addShader("phong", GL_GEOMETRY_SHADER, scene_capture_geom_shader);
		addShader("phong", GL_FRAGMENT_SHADER, phong_lfp_frag_shader);

		addShader("particle", GL_VERTEX_SHADER, particle_identity_vert_shader);
		addShader("particle", GL_FRAGMENT_SHADER, identity_frag_shader);

		addShader("particle_lfp", GL_VERTEX_SHADER, particles_lfp_vert_shader);
		addShader("particle_lfp", GL_FRAGMENT_SHADER, phong_lfp_frag_shader);
		
	}

	void init() override {
		_fullScreen = true;
		_modelHeight = 5.0f;
		initDefaultCamera();
		//activeCamera().setMode(Camera::SPECTATOR);
		//activeCamera().lookAt({ 0, 0, 5 }, vec3{ 0 }, { 0, 1, 0 });


		for (int i = 0; i < 4; i++) {
			string loc = "00" + to_string(5 + i);
			string root = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\" + loc + "\\";

			vector<string> skyTextures = vector<string>{
				"right.jpg", "left.jpg",
				"top.jpg", "bottom.jpg",
				"front.jpg", "back.jpg"
			};

			transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
				return root + path;
			});
			skyboxes.push_back(SkyBox::create(skyTextures, 0, *this));
		}

		skybox = skyboxes[nextSkyBox];
		ground = new Plane(4, 4, 2000, 2000, 200);
		floor = new Floor(this);
		cube = new Cube(5, RED);
		
		sun = Sphere{ 1, 50, 50, vec4(0.96, 0.99, 0.96, 1) };
		ball = Sphere{ 0.2, 10, 10 };
	//	ballPos = vec3(0.289, 1.58, -0.029);
		ballPos = vec3(0.504, 1.6, 0);
		auto lightPos = vec3{ 0, 10, 10 };
		light[0].position = vec4{ lightPos, 1 };
		auto znear = activeCamera().getNear();
		auto zfar = activeCamera().getFar();
		shadowMap = new OminiDirectionalShadowMap{ 5, lightPos, 2048, 2048, 1, zfar };
		float dist = length(lightPos);
		dist *= 1.5f;
		DirectionalShadowMap::LightView lv;
		lv.view = lookAt(lightPos, vec3(0), { 0, 1, 0 });
		lv.projection = ortho(-dist, dist, -dist, dist, 1.0f, 20.f);
		shader("phong")([&] {
			send("lightSpaceView", lv.view);
		});
		//shadowMap = new DirectionalShadowMap{ 4, lv, 2048, 2048 };

		// TODO reset background color after framebuffer execution


		auto d = normalize(vec3(-3.61, 2.93, 0) - ballPos);
		ballPos = ballPos + 0.5f * d;
		dir = new Vector{ d * 5.0f, ballPos };

		registry = new ParticleRegistry{ 1000 };
		int count = registry->particles().count();

		cannonBall = new CannonBall{ this, *registry };
		cannon = new Cannon(this, *registry);


		integrator = new ParticleIntegrator{ registry->particles() };
	//	cannonBallReset = new CannonBallReset{ registry->particles(), *this };
		gravity = new Gravity{ 1, vec3{0, -9.81, 0}, registry->particles() };

		addCompute(gravity);
		addCompute(integrator);
//		addCompute(cannonBallReset);

		shadowMap->capture([&] {
			floor->render(true);
			cannon->render(true);
			cannonBall->render(true);
		});
		setBackGroundColor(BLACK);
		setForeGroundColor(YELLOW);
	}

	void display() override {
		//glDisable(GL_CULL_FACE);
		skybox->render();
		shader("flat")([&](Shader& s) {
			//send(activeCamera(), translate(mat4(1), vec3(light[0].position)));
			//send("checker", false);
			//shade(sun);
			
			//send(activeCamera(), translate(mat4(1), ballPos));
			//shade(ball);

			//send(activeCamera());
			//shade(dir);
		});

	//	shadowMap->update(light[0].position.xyz);
		shadowMap->capture([&] {
			floor->render(true);
			cannon->render(true);
			cannonBall->render(true);
		});

		shader("phong")([&]() {
			send(shadowMap);
			send("shadowOn", true);
			send("camPos", activeCamera().getPosition());
			send("lightPos", light[0].position.xyz);
			send("lightColor", vec3(1));
			floor->render();
			});


		shader("particle_lfp")([&]() {
			send(shadowMap);
			send("shadowOn", true);
			send("camPos", activeCamera().getPosition());
			send("lightPos", light[0].position.xyz);
			send("lightColor", vec3(1));
			//floor->render();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			send("useNormalMap", nm);
			cannon->render();
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});

		cannonBall->render();

		sbr << "ball position: " << ballPos;
		if (nm) {
			sbr << "\nNormal mapping: on";
		}
		else {
			sbr << "\nNormal mapping: off";
		}
		sbr << "\nCurrent Shot Type: " << cannonBall->shotType();
		sFont->render(sbr.str(), 10, textOffset + 10);
	}

	void update(float t) override {

	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case 'i' : 
				getActiveCameraController().move(false);
				interact = true;
				break;
			case 'c':
				getActiveCameraController().move(true);
				interact = false;
				break;
			case 'n':
				nm = !nm;
				break;
			case 'f':
				cannonBall->fire();
				break;
			case ' ':
				cannonBall->nextShotType();
				break;
			case 'o':
				nextSkyBox++;
				nextSkyBox %= skyboxes.size();
				skybox = skyboxes[nextSkyBox];
				break;
			}


			if (interact) {
				auto camView = activeCamera().getViewMatrix();
				vec3 ballCam = vec3(camView * vec4(ballPos, 1));
				switch (key.value()) {
				case 'w':
					ballCam.y += 0.01;
					break;
				case 's':
					ballCam.y -= 0.01;
					break;
				case 'a':
					ballCam.x -= 0.01;
					break;
				case 'd':
					ballCam.x += 0.01;
				}
				ballPos = vec3(inverse(camView) * vec4(ballCam, 1));
			}
		}
	}



protected:
	SkyBox* skybox;
	vector<SkyBox*> skyboxes;
	Plane* ground;
	Cube* cube;
	Cannon* cannon;
	CannonBall* cannonBall;

	Floor* floor;
	ShadowMap* shadowMap;
	Vector* dir;
	Sphere sun;
	Sphere ball;
	vec3 ballPos;
	ParticleIntegrator* integrator;
	ParticleRegistry* registry;
	Gravity* gravity;
	CannonBallReset* cannonBallReset;
	bool nm = true;
	bool interact = false;
	int nextSkyBox = 0;
};
