#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../GLUtil/include/ncl/ray_tracing/RayGenerator.h"
#include "../GLUtil/include/ncl/ray_tracing/model.h"
#include "../GLUtil/include/ncl/ray_tracing/Ray.h"
using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
namespace rt = ray_tracing;

#pragma pack(push, 1)
struct Stack {
	int size = 0;
	int top = 0;
	int data[64];
};
#pragma pack(pop)

const float DAY_LIGHT_ILLUMINANCE = 64000;
const int MaxSpheres = 250;
const int MAX_BOUNCES = 10;

class RayTracer : public Compute {
public:
	RayTracer(Scene& scene, StorageBufferObj<std::vector<rt::Ray>>& rays)
		:Compute(vec3{ scene.width() / 32.0f, scene.height() / 32.0f, 1.0f }
			, vector<Image2D>{ Image2D(scene.width(), scene.height(), GL_RGBA32F, "image", 0) }
	, & scene.shader("whitted_raytracer"))
		, rays{ rays }
		, scene{ scene }{

		auto& cam = scene.getActiveCameraController();
		worldRadius = (cam.getFar() - cam.getNear()) * 0.5;
		worldCenter = vec3(0);


		vector<string> skyTextures = vector<string>{
			"right.jpg", "left.jpg",
			"top.jpg", "bottom.jpg",
			"front.jpg", "back.jpg"
		};

		string root = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
			});

		skybox = SkyBox::create(skyTextures, 0, scene);
		auto numRays = scene.width() * scene.height();

		checkerboard = new CheckerBoard_gpu(255, 255, WHITE, GRAY, 1, "checker");
		checkerboard->compute();
		glBindTexture(GL_TEXTURE_2D, checkerboard->images().front().buffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		checkerboard->images().front().renderMode();

		initObjs();
		scene.shader("whitted_raytracer")([&] {
			plane_ssbo.sendToGPU(true);
			light_ssbo.sendToGPU(true);
			sphere_ssbo.sendToGPU(true);
			material_ssbo.sendToGPU(true);
			send("worldCenter", worldCenter);
			send("worldRadius", worldRadius);
			send("numSpheres", numSpheres);
		});
	}

	void initObjs() {
		auto materials = std::vector<rt::Material>{};

		rt::Plane plane;
		plane.n = { 0, 1, 0 };
		plane.d = 0;
		plane.id = 0;
		plane.matId = materials.size();

		rt::Material m;
		m.ambient = vec4(0);
		m.diffuse = vec4(1, 0, 0, 1);
		m.ior = 0;
		m.bsdf[0] = rt::BSDF_DIFFUSE;
		m.nBxDfs += 1;
		m.kr = vec4(0.8, 0.8, 0.8, 1.0);
		materials.push_back(m);

		auto planes = vector<rt::Plane>{};
		planes.push_back(plane);
		plane_ssbo = StorageBufferObj<vector<rt::Plane>>{ planes, 4 };
		initLights();
		initSpheres(materials);

		material_ssbo = StorageBufferObj<vector<rt::Material>>{ materials, 3 };
	}

	void initLights() {
		auto lights = vector<rt::LightSource>{};
		
		rt::LightSource light;
		light.flags = rt::POINT_LIGHT;
	//	light.flags = rt::DISTANT_LIGHT;
		light.I = vec4(vec3(1), 80);
	//	light.I = vec4(vec3(1), 1);
		light.lightToWorld = translate(mat4(1), { 0, 10, 10 });
		light.worldToLight = inverse(light.lightToWorld);
		light.position = light.lightToWorld * vec4(0, 0, 0, 1);
	//	light.position = vec4(0, 1, 0, 1);
		lights.push_back(light);
		
		light_ssbo = StorageBufferObj<vector<rt::LightSource>>{ lights, 2 };
	}

	void initSpheres(vector< rt::Material>& materials) {
		auto spheres = std::vector<rt::Sphere>{};
		vec2 radius = { 0.3, 2.0 };
		float placementRadius = 20;
		auto rng = rngReal(0, 1);
		auto rngIOR = rngReal(1.2, 2.417);
		auto rngShine = rngReal(50, 100);
		auto rngIndex = rngInt(0, 3);

		int bsdfs[4] = { rt::SPECULAR_TRANSMISSION, rt::SPECIULAR_REFLECT, rt::BSDF_DIFFUSE, rt::FRESNEL_SPECULAR };

		for (int i = 0; i < MaxSpheres; i++) {

			rt::Material m;
			m.ambient = vec4(0);
			m.diffuse = randomColor();
			m.specular = vec4(1);
			m.shine = rngShine();
			m.kt = randomColor();
			m.kt = vec4(1);
			m.kr = vec4(0.8, 0.8, 0.8, 1.0);
			m.ior = rngIOR();
		//	m.bsdf[0] = SPECULAR_TRANSMISSION;
		//	m.bsdf[0] = rt::BSDF_DIFFUSE;
			m.bsdf[0] = bsdfs[rngIndex()];
			m.nBxDfs += 1;

			rt::Sphere s;
			s.radius = radius.x + rng() * (radius.y - radius.x);
			s.radius = radius.x + rng() * (radius.y - radius.x);
			float r = sqrt(rng());
			vec2 u = vec2(r * cos(rng() * two_pi<float>()), r * sin(rng() * two_pi<float>())) * placementRadius;
			s.center = vec4(0);
			s.objectToWorld = translate(mat4(1), vec3{ u.x, s.radius, u.y });
			s.worldToObject = inverse(s.objectToWorld);


			for (auto other : spheres) {
				float minDist = s.radius + other.radius;
				vec3 aCenter = (s.objectToWorld * vec4(0, 0, 0, 1)).xyz;
				vec3 oCenter = (other.objectToWorld * vec4(0, 0, 0, 1)).xyz;
				vec3 d = aCenter - oCenter;
				if (dot(d, d) < minDist * minDist) goto outter;
			}
			s.matId = materials.size();
			s.id = spheres.size();
			materials.push_back(m);
			spheres.push_back(s);


		outter:
			continue;
		}
		numSpheres = spheres.size();
		sphere_ssbo = StorageBufferObj<vector<rt::Sphere>>{ spheres, 5 };
	}

	void preCompute() {
		//auto& light = light_ssbo.get()[0];
		//light.lightToWorld = translate(mat4(1), { scene.activeCamera().getPosition() });
		//light.worldToLight = inverse(light.lightToWorld);
		//light.position = light.lightToWorld * vec4(0, 0, 0, 1);
		//light_ssbo.sendToGPU(true);
		rays.sendToGPU(false);
		send("bounces", _bounces);
		glBindTextureUnit(0, skybox->buffer);
		auto& img = checkerboard->images().front();
		glBindTextureUnit(1, img.buffer());
		send("camPos", scene.activeCamera().getPosition());
	}

	void processInput(const Key& key) {
		if (key.released()) {
			switch (key.value()) {
			case '+':
			case '=':
				_bounces += 1;
				break;
			case '-':
				_bounces -= 1;
				break;
			}
			_bounces = glm::clamp(_bounces, 0, MAX_BOUNCES);
		}
	}

	int bounces() {
		return _bounces;
	}

	void render() {
		skybox->render();
	}

private:
	StorageBufferObj<std::vector<rt::Ray>>& rays;
	StorageBufferObj<vector<rt::Plane>> plane_ssbo;
	StorageBufferObj<vector<rt::LightSource>> light_ssbo;
	StorageBufferObj<vector<rt::Sphere>> sphere_ssbo;
	StorageBufferObj<vector<rt::Material>> material_ssbo;
	CheckerBoard_gpu* checkerboard;
	SkyBox* skybox;
	Scene& scene;
	vec3 worldCenter;
	float worldRadius;
	int numSpheres;
	int numPlanes;
	int _bounces = MAX_BOUNCES;
};


class RayTracingScene : public Scene {
public:
	RayTracingScene() :Scene("Ray tracing scene", 1024, 1024) {
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
	//	addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);

		addShader("stack_test", GL_VERTEX_SHADER, screen_vert_shader);
		_debug = true;
	}

	void init() override {
		initDefaultCamera();
		camera_ssbo = gl::StorageBufferObj<rt::Camera>{ rt::Camera{} };
		rayGenerator = new rt::RayGenerator{ *this, camera_ssbo };
		raytracer = new RayTracer{ *this, rayGenerator->getRaySSBO() };
		quad = ProvidedMesh{ screnSpaceQuad() };
		setBackGroundColor(BLACK);
	}

	bool once = true;
	void display() override {
		rayGenerator->compute();

		raytracer->compute();
		

		shader("screen")([&] {
			raytracer->images().front().renderMode();
			glBindTextureUnit(0, raytracer->images().front().buffer());
			shade(quad);
		});

		//shader("stack_test")([&] {
		//	shade(quad);
		//});
		sFont->render("Bounces: " + to_string(raytracer->bounces()), 0, 70);
	}

	void processInput(const Key& key) override {
		raytracer->processInput(key);
	}

private:
	rt::RayGenerator* rayGenerator;
	RayTracer* raytracer;
	Logger& logger = Logger::get("ray");
	StorageBufferObj<rt::Camera> camera_ssbo;
	ProvidedMesh quad;
	rt::Camera* rCamera;
};

//template<>
//struct ncl::gl::ObjectReflect<std::vector<Stack>> {
//
//	static GLsizeiptr sizeOfObj(std::vector<Stack>& stacks) {
//		auto size = sizeof(Stack);
//		return GLsizeiptr(size * stacks.size());
//	}
//
//	static void* objPtr(std::vector<Stack>& stacks) {
//		return &stacks[0];
//	}
//
//	static GLsizeiptr sizeOf(int count) {
//		return GLsizeiptr(count * sizeof(Stack));
//	}
//};