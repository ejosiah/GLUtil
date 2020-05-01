#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

const int Power = 0;
const int Intensity = 1; // power/sr
const int Illuminance = 2; // power /area
const int Luminance = 3; // power /(sr * area)

const int Point = 0;
const int Spot = 1;
const int Direct = 2;
const int Sun = 3;
const int AreaSphere = 4;
const int AreaDisk = 5;
const int AreaRectangle = 6;

struct Light {
	int unit;
	int type;
	float value;
	vec3 position;
	vec3 color;
	vec3 normal = { 0, -1, 0 };
	Shape* shape;
	float radius;
};


class AreaLightScene : public Scene {
public:
	AreaLightScene() :Scene("AreaLigth") {
		_modelHeight = 0.5;
		_fullScreen = true;
	}

	void init() override {
		lightColor = { 1.0f, 248 / 255.0f, 171 / 225.0f, 1.0f };
		initDefaultCamera();
		mat4 model{ 1 };
		model = scale(model, {8, 4, 10});
		model = translate(model, { 0, 0.5, 0 });
		cube = new Cube{ 1.0f, model, GRAY };

		orb = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 0.5);
		auto midpoint = (orb->bound->max() - orb->bound->min()) * 0.5f;
		offset = dot(midpoint, { 0, 1, 0 });
		activeCamera().lookAt({ 0, 0, 3 }, vec3(0), { 0, 1, 0 });
		activeCamera().setMode(Camera::Mode::FIRST_PERSON);

		light.position = {2.9, 2,  0};
		light.type = AreaSphere;
		light.unit = Luminance;
		light.value = 700;
		light.color = lightColor.xyz;
		light.radius = 0.5f;
		light.shape = new Sphere(light.radius, 50, 50, WHITE);

		model = rotate(model, glm::radians(-90.0f), { 0, 0, 1 });
		light.normal = mat3(model) * light.normal;

		sphere = new Sphere(0.2, 50, 50, RED);
		createDisk();
	}

	void createDisk() {
		Mesh mesh;
		float segments = 100;
		float dt = two_pi<float>() / segments;
		float PI = pi<float>();
		float r = 1.0f;
		for (float t = -PI; t <= PI; t += dt) {
			mesh.positions.push_back(vec3{ r * cos(t), 1, r * sin(t) });
			mesh.colors.push_back(lightColor);
		}
		mesh.primitiveType = GL_TRIANGLE_FAN;
		disk = new ProvidedMesh(mesh);
	}

	void display() override {
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader("area_light")([&]() {
			send(activeCamera());
			shade(cube);

			mat4 model = translate(mat4(1), { 0, offset, 0 });
			model = rotate(model, half_pi<float>(), { 0, 1, 0 });
			send(activeCamera(), model);
			shade(orb);

			//mat4 model = translate(mat4(1), { -3, 2, 0 });
			//send(activeCamera(), model);
			//shade(sphere);
			send("light.normal", light.normal);
			send("eyes", activeCamera().getPosition().xyz);
			_send(light);
		});

		shader("flat")([&]() {
			auto model = translate(mat4(1), light.position);
			model = rotate(model, glm::radians(-90.0f), { 0, 0, 1 });
			send(activeCamera(), model);
			//shade(light.shape);
			shade(disk);
		});
	}

	void _send(Light& light) {
		send("light.type", light.type);
		send("light.unit", light.unit);
		send("light.position", light.position);
		send("light.value", light.value);
		send("light.color", light.color);
		send("light.radius", light.radius);
	}

	void resized() override {
		//activeCamera().perspective(65.0f, aspectRatio, 0.1f, 100.0f);
	}

private:
	Cube* cube;
	Model* orb;
	Light light;
	Sphere* sphere;
	ProvidedMesh* disk;
	vec4 lightColor;
	float offset;
};