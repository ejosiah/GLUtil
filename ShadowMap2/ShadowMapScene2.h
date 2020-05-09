#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "ShadowMap.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ShadowMapScene2 : public Scene {
public:
	ShadowMapScene2():Scene("Shadow map 2"){}

	void init() override {
		initDefaultCamera();
		camera = &activeCamera();
		camera->setMode(Camera::FIRST_PERSON);
		camera->setPosition(vec3(0, 0.5, 3));
		createPlane();
		armadillo = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\Armadillo.obj", true, 2);
		vec3 d = armadillo->bound->max() - armadillo->bound->min();
		float offset = dot({ 0, 1, 0 }, d) * 0.5f;
		armadillioModel = translate(mat4{ 1 }, { 0, offset, 0 });
		armadillioModel = rotate(armadillioModel, pi<float>(), { 0, 1, 0 });
		lightPos = { 0, 3, 3 };
		lightObj = new Sphere{ 0.2, 50, 50, YELLOW };
		obj = new Sphere{ 1, 50, 50, RED };

		shadowMap = ShadowMap0{
			4,
			{
				lookAt(lightPos, vec3(0), {0, 1, 0}),
				perspective(radians(90.0f), float(_width)/_height, 1.0f, 25.f)
			},
			_width,
			_height
		};

		setBackGroundColor(BLACK);
	}

	void display() override {
		//shadowMap.render();
		cam.view = lookAt(lightPos, vec3(0), { 0, 1, 0 });
		cam.projection = perspective(radians(90.0f), float(_width) / _height, 1.0f, 25.f);
		shader("phong")([&](Shader& s) {
			send("lightPos", lightPos);
			send("camPos", camera->getPosition());

			shadowMap.sendTo(s);

			plane->defautMaterial(true);
			send(*camera, mat4{ 1 });
	//		cam.model = mat4{ 1 };
	//		send(cam);
			shade(plane);

			armadillo->defautMaterial(true);
		//	cam.model = armadillioModel;
		//	send(cam);
			send(*camera, armadillioModel);
			shade(armadillo);
		});

		shader("flat")([&] {
			send(*camera, translate(mat4{ 1 }, lightPos));
			shade(lightObj);
		});

		shadowMap.capture([&] {
			plane->defautMaterial(false);
			send("M", mat4{ 1 });
			shade(plane);

			armadillo->defautMaterial(false);
			send("M", armadillioModel);
			shade(armadillo);
		});
		setBackGroundColor(BLACK);
	}

	void createPlane() {
		loadPlankMaterial();
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
		plane->defautMaterial(true);
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
	Model* armadillo;
	Sphere* lightObj;
	Sphere* obj;
	vec3 lightPos;
	mat4 armadillioModel;
	ProvidedMesh* plane;
	ShadowMap0 shadowMap;
	pbr::TextureMaterial plankMaterail;
	Camera* camera;
	Cube cube;
};