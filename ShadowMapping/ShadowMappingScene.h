#pragma once
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "../GLUtil/include/ncl/gl/ShadowMap.h"
#include "../GLUtil/include/ncl/gl/ViewFrustum.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ShadowMappingScene : public Scene {
public:
	ShadowMappingScene() :Scene("ShadowMapping") {
		_modelHeight = 2.8;
		_fullScreen = false;
		camInfoOn = true;
	}

	void init() override {
		loadPlankMaterial();
		setForeGroundColor(BLACK);
		initDefaultCamera();
		activeCamera().collisionTestOff();
		activeCamera().setMode(Camera::FIRST_PERSON);
			sponza = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\Sponza\\sponza.obj", false, 20);
			//cornell = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\cornell\\cornell_box.obj", true, 10);
			current = sponza;
			//float y = current->bound->min().y;
			//offset = std::abs(y);
			//activeCamera().setPosition(vec3(0));
			updateLightPosition();
			//lightView = lookAt(lightPos, vec3(0, 0, 0), { 0, 1, 0 });

			//auto bottom = current->bound->min();
			//auto top = current->bound->max();

			//float near_plane = bottom.y;
			//float far_plane = top.y;
			//vec3 d = top - bottom;
			//float h = dot({ 0, 1, 0 }, d);
			//float w = dot({ 1, 0, 0 }, d);
			//float l = dot({ 0, 0, 1 }, d);

		lightPos = { -2, 4, -1 };
		lightPos = activeCamera().getPosition();
		shadowMap = DirectionalShadowMap{
			4,
			{
				lookAt(lightPos, vec3(0), {0.0f, 1.0f, 0.0f}),
				ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.5f),
				//perspective(radians(90.0f), 1.0f, 1.0f, 25.0f),
				false
			},
			1024,
			1024
		};

		//	viewFrustum = ViewFrustum::ortho(lightPos, vec3(0), { 0.0f, 1.0f, 0.0f }, -10, 10, -10, 10, 1, 7);
		viewFrustum = ViewFrustum::perspective(lookAt(lightPos, vec3(0), { 0.0f, 1.0f, 0.0f }), radians(90.0f), 1.0f, 1.0f, 800.0f);

		vec3 Y = { 0, 1, 0 };
		vec3 fwd = normalize(-lightPos);
		vec3 rig = normalize(cross(fwd, Y));
		vec3 cY = normalize(cross(rig, fwd));

		up = new Vector(Y, lightPos, 1, YELLOW);
		forward = new Vector(fwd, lightPos, 1, RED);
		right = new Vector(rig, lightPos, 1, GREEN);
		camUp = new Vector(cY, lightPos, 1, BLUE);
		//	lightPos = vec3(0);

			//createPlane();
			//createCube();
		activeCamera().setPosition({ 0, 0, 3 });
		lightPos = { 0, 2.43, 0.6 };
		pointShadowMap = OminiDirectionalShadowMap{ 5, lightPos, 2048, 2048 };
		lightObj = new Sphere(0.3, 20, 20, WHITE);
	//	lightPos = vec3(0);
		createCubeForPointShadow();

	}

	void updateLightPosition() {
		if (current == cornell) {
			auto id = cornell->getMeshId("light");
			if (id != -1) {
				vec3 _min = vec3{ numeric_limits<float>::max() };
				vec3 _max = vec3{ numeric_limits<float>::min() };
				cornell->get<vec3>(id, 0, [&](GlBuffer<vec3> buffer) {
					for (auto itr = buffer.begin(); itr != buffer.end(); itr++) {
						auto v = *itr;
						_min = glm::min(v, _min);
						_max = glm::max(v, _max);
					}
					});
				//	lightPos = ((_min + _max) * 0.5f) - vec3{0, 0.2, 0};
				lightPos = ((_min + _max) * 0.5f);
			}
		}
		else {
			//lightPos = activeCamera().getPosition();
			lightPos = (sponza->bound->min() + sponza->bound->max()) * 0.5f;
		}
	}

	void display() override {
		//shadowMap.render();
		pointShadowMap.update(lightPos);
		pointShadowMap.capture([&] {
			current->defautMaterial(false);
			//cube->defautMaterial(false);
			//shade(cube);
			shade(current);
		});

		//	vec3 camPos = { 0.0f, 0.0f, 3.0f };
		vec3 camPos = activeCamera().getPosition();
		//	lightPos = camPos;

		shader("phong")([&] {
			//	plane->defautMaterial(true);
			current->defautMaterial(true);
			cube->defautMaterial(true);
			//	current->defautMaterial(true);
			send(activeCamera());
			//	send("lightSpaceView", shadowMap.lightViewMatrix());
			send(pointShadowMap);
			send("lightPos", lightPos);
			send("camPos", camPos);
			send("shadowOn", true);

			//glBindTextureUnit(0, plankMaterail.albedo());
			//glBindTextureUnit(1, plankMaterail.albedo());
			//glBindTextureUnit(2, plankMaterail.ambientOcclusion());
			//glBindTextureUnit(3, plankMaterail.normal());
		//	shade(plane);
			//shade(cube);
				shade(current);
			});

		//pointShadowMap.update(activeCamera().getPosition());
		//pointShadowMap.capture([&] {
		//	//plane->defautMaterial(false);
		//	//cube->defautMaterial(false);
		//	//shade(plane);
		//	//shade(cube);
		//	current->defautMaterial(false);
		//	shade(current);
		//});

		//pointShadowMap.update(lightPos);


		shader("flat")([&] {
			auto model = translate(mat4{ 1 }, lightPos);
			send(activeCamera(), model);
			shade(lightObj);


			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//send(activeCamera());
			//shade(up);
			//shade(forward);
			//shade(right);
			//shade(camUp);
			//shade(viewFrustum);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			});
		setBackGroundColor(BLACK);
	}

	void update(float t) override {
		elapsedTime += t;
		//lightPos.z = ::sin(elapsedTime * 0.5f) * 3.0f;
	}

	void resized() override {
		activeCamera().perspective(65.0f, aspectRatio, 0.1, 5000);
	}

	void createPlane() {
		Mesh mesh;
		mesh.positions.emplace_back(25.0f, -0.5f, 25.0f);
		mesh.positions.emplace_back(-25.0f, -0.5f, 25.0f);
		mesh.positions.emplace_back(-25.0f, -0.5f, -25.0f);

		mesh.positions.emplace_back(25.0f, -0.5f, 25.0f);
		mesh.positions.emplace_back(-25.0f, -0.5f, -25.0f);
		mesh.positions.emplace_back(25.0f, -0.5f, -25.0f);

		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);

		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
		mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);

		mesh.uvs[0].emplace_back(25.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 25.0f);

		mesh.uvs[0].emplace_back(25.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 25.0f);
		mesh.uvs[0].emplace_back(25.0f, 10.0f);

		mesh.primitiveType = GL_TRIANGLES;
		mesh.colors = vector<vec4>{ 6, BLUE };

		mesh.material.ambientMat = plankMaterail.albedo();
		mesh.material.diffuseMat = plankMaterail.albedo();
		mesh.material.bumpMap = plankMaterail.normal();

		plane = new ProvidedMesh(mesh);
	}

	void createCube() {
		vector<mat4> models;
		auto model = mat4{ 1 };
		model = translate(model, { 0.0f, 1.5, 0.0 });
		model = scale(model, vec3(0.5f));
		models.push_back(model);

		model = translate(mat4{ 1 }, { 2.0f, 0.0f, 1.0f });
		model = scale(model, vec3(0.5));
		models.push_back(model);

		model = translate(mat4{ 1.0 }, { -1.0f, 0.0f, 2.0f });
		model = rotate(model, radians(60.0f), normalize(vec3{ 1.0f, 0.0f, 1.0f }));
		model = scale(model, vec3(0.25));
		models.push_back(model);

		cube = new Cube(2, RED, models, false);
		auto& material = cube->material();
		material.ambientMat = plankMaterail.albedo();
		material.diffuseMat = plankMaterail.albedo();
		material.bumpMap = plankMaterail.normal();

	}


	void createCubeForPointShadow() {
		vector<mat4> models;
		auto model = mat4{ 1 };

		model = scale(model, vec3(5));
		models.push_back(model);

		model = translate(mat4{ 1 }, { 4.0f, -3.5f, 0.0f });
		model = scale(model, vec3(0.5f));
		models.push_back(model);

		model = translate(mat4{ 1 }, { 2.0f, 3.0f, 1.0f });
		model = scale(model, vec3(0.75f));
		models.push_back(model);

		model = translate(mat4{ 1 }, { -3.0f, -1.0f, 0.0f });
		model = scale(model, vec3(0.5));
		models.push_back(model);

		model = translate(mat4{ 1 }, { -1.5f, 1.0f, 1.5f });
		model = scale(model, vec3(0.5f));
		models.push_back(model);

		model = translate(mat4{ 1 }, { -1.5f, 2.0f, -3.0f });
		model = rotate(model, radians(60.0f), normalize(vec3{ 1.0f, 0.0f, 1.0f }));
		model = scale(model, vec3(0.75f));
		models.push_back(model);

		cube = new Cube(2, RED, models, false);
		auto& material = cube->material();
		material.ambientMat = plankMaterail.albedo();
		material.diffuseMat = plankMaterail.albedo();
		material.bumpMap = plankMaterail.normal();
	}

	void loadPlankMaterial() {
		plankMaterail = pbr::TextureMaterial{
			"Red Plank",
			pbr::Material::Albedo{"C:\\Users\\Josiah\\OneDrive\\media\\textures\\wood_floor\\wood_floor.jpg"},
			pbr::Material::Normal{"C:\\Users\\Josiah\\OneDrive\\media\\textures\\wood_floor\\wood_floor_nm.jpg"},
			pbr::Material::Metalness{1.0f},
			pbr::Material::Roughness{1.0f},
			pbr::Material::AmbientOcculusion{1.0f}
		};
	}

private:
	float elapsedTime = 0.0f;
	float offset;
	Model* sponza;
	Model* cornell;
	Model* current;
	Cube* cube;
	Sphere* lightObj;
	ProvidedMesh* plane;
	mat4 lightView;
	vec3 lightPos;
	DirectionalShadowMap shadowMap;
	OminiDirectionalShadowMap pointShadowMap;
	pbr::TextureMaterial plankMaterail;
	ViewFrustum* viewFrustum;
	Vector* camUp;
	Vector* up;
	Vector* forward;
	Vector* right;

};