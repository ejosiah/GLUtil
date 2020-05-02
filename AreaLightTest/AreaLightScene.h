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

const int SPHERE_SHAPE = 1000 << 0;
const int DISK_SHAPE = 1000 << 1;
const int RECTANGLE_SHAPE = 1000 << 2;
const int TUBE_SHAPE = 1000 << 3;

struct Sphere_L {
	float radius;
	vec3 center;
};

struct Disk_L {
	float radius;
	vec3 center;
};

struct Rectangle_L {
	float width, height;
};

struct Tube_L {
	float height;
	float radius;
	vec3 center;
};


struct Light {
	int unit;
	int type;
	int shapeId;
	float value;
	vec3 position = glm::vec3(0);
	vec3 color;
	vec3 normal = { 0, -1, 0 };
	Shape* shape;
	float radius;
	mat4 localToWorld = glm::mat4{ 1 };
};

struct RenderEq {
	vec3 V;
	vec3 N;
	vec3 L;
	vec3 Na;
	vec3 R;
	vec3 P;
};


class AreaLightScene : public Scene {
public:
	AreaLightScene() :Scene("AreaLigth") {
		_modelHeight = 0.5;
		_fullScreen = false;
	}

	void init() override {
		lightColor = { 1.0f, 248 / 255.0f, 171 / 225.0f, 1.0f };
		initDefaultCamera();
		mat4 model{ 1 };
		model = scale(model, {8, 4, 10});
		model = translate(model, { 0, 0.5, 0 });
		cube = new Cube{ 1.0f, model, GRAY };

		orb = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 0.5);
		auto midpoint = (orb->bound->max() - orb->bound->min()) * 0.5f;
		offset = dot(midpoint, { 0, 1, 0 });
		activeCamera().setMode(Camera::Mode::FIRST_PERSON);
		activeCamera().lookAt({ 0, 2, 3 }, vec3(0), { 0, 1, 0 });
		auto& camm = activeCamera();
		light.type = AreaSphere;
		light.unit = Luminance;
		light.value = 700;
		light.color = lightColor.xyz;
		light.radius = 0.5f;
		light.shape = new Sphere(light.radius, 50, 50, WHITE);

		light.localToWorld = translate(mat4(1), { 3.9, 2,  0 });
		light.localToWorld = rotate(light.localToWorld, glm::radians(-90.0f), { 0, 0, 1 });
		rectangleLightShape.width = 2.0f;
		rectangleLightShape.height = 2.0f;
		light.shapeId = 2 | RECTANGLE_SHAPE;

		sphere = new Sphere(0.2, 50, 50, RED);
		pos = new Sphere(0.1, 10, 10, RED);
		createDisk();
		createRectangle();
		createTube();
		initVectors();
	}

	void initVectors() {
		vec3 eyes = activeCamera().getPosition();
		vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;
		vec3 l = lightPos - x;
		vec3 v = normalize(activeCamera().getPosition() - x);
		vec3 n = { 0, 1, 0 };
		vec3 r = reflect(v, n);
		mat3 lightToWorld3 = mat3(transpose(inverse(light.localToWorld)));
		vec3 na = normalize(lightToWorld3 * light.normal);

		L = new Vector{ l, x, 1.0f, BLUE };
		V = new Vector{ v, x, 1.0f, GREEN };
		N = new Vector{ n, x, 1.0f, RED};
		Na = new Vector{ na, lightPos, 1.0f, RED };
		R = new Vector{ r, x , 1.0, YELLOW };
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

	void createRectangle() {
		Mesh mesh;
		mesh.positions.emplace_back(1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, -1.0f);
		mesh.positions.emplace_back(1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, -1.0f);
		mesh.positions.emplace_back(1.0f, -0.0f, -1.0f);
		mesh.colors = vector<vec4>(6, lightColor);
		mesh.primitiveType = GL_TRIANGLES;

		rectangle = new ProvidedMesh(mesh);
	}

	void createTube() {
		vector<Mesh> meshes;
		auto cylinder = new Cylinder(0.2f, 1.0f);
		auto cap0 = new Hemisphere(0.2f);
		Mesh mesh = cap0->getMeshes().at(0);
		Mesh frontCap = mesh;
		frontCap.name = "front_cap";
		mat4 model = translate(mat4{ 1 }, { 0, 0, 0.5f });
		model = rotate(model, half_pi<float>(), { 1, 0, 0 });

		std::transform(frontCap.positions.begin(), frontCap.positions.end(), frontCap.positions.begin(), [&](auto p) {
			return vec3((model * vec4(p, 1.0f)));
		});
		std::transform(frontCap.normals.begin(), frontCap.normals.end(), frontCap.normals.begin(), [&](auto n) {
			return mat3(model) * n;
		});
		Mesh backCap = frontCap;
		backCap.name = "back_cap";
		model = rotate(mat4{ 1 }, pi<float>(), { 0, 1, 0 });

		std::transform(backCap.positions.begin(), backCap.positions.end(), backCap.positions.begin(), [&](auto p) {
			return vec3((model * vec4(p, 1.0f)));
		});
		std::transform(backCap.normals.begin(), backCap.normals.end(), backCap.normals.begin(), [&](auto n) {
			return mat3(model) * n;
		});

		Mesh body = cylinder->getMeshes().at(0);
		body.name = "body";
		model = translate(mat4{ 1 }, { 0.0f, 0.0f, 0.5f });

		std::transform(body.positions.begin(), body.positions.end(), body.positions.begin(), [&](auto p) {
			return vec3((model * vec4(p, 1.0f)));
			});

		vector<vec4> colors{ body.positions.size(), GRAY };
		frontCap.colors = vector<vec4>{ colors };
		body.colors = colors;
		backCap.colors = vector<vec4>{ colors };

		meshes.push_back(frontCap);
		meshes.push_back(body);
		meshes.push_back(backCap);
		delete cylinder;
		tube = new ProvidedMesh(meshes);
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
			
			send("eyes", activeCamera().getPosition().xyz);
			_send(light);

			//model = translate(mat4(1), { 0, 1, 0 });
			//send(activeCamera(), model);
			//shade(tube);
		});

		shader("flat")([&]() {
			send(activeCamera(), light.localToWorld);
			//shade(light.shape);
			shade(rectangle);

			//send(activeCamera());
			//shade(pos);
			//shade(L);
			//shade(V);
			//shade(N);
			//shade(Na);
			//shade(R);
		});
	}

	void _send(Light& light) {
		send("light.type", light.type);
		send("light.unit", light.unit);
		send("light.position", light.position);
		send("light.value", light.value);
		send("light.color", light.color);
		send("light.radius", light.radius);
		send("light.localToWorld", light.localToWorld);
		send("light.normal", light.normal);
		send("light.shapeId", light.shapeId);
		send("rectangleLights[2].width", rectangleLightShape.width);
		send("rectangleLights[2].height", rectangleLightShape.height);
	}

	void update(float t) {
		vec3 v = normalize(activeCamera().getPosition() - x);
		vec3 r = -reflect(v, { 0, 1, 0 });
		V->update(v);
		R->update(r);
	}

	void resized() override {
		//activeCamera().perspective(65.0f, aspectRatio, 0.1f, 100.0f);
	}

private:
	Cube* cube;
	Model* orb;
	Light light;
	Sphere* sphere;
	Sphere* pos;
	ProvidedMesh* disk;
	ProvidedMesh* rectangle;
	Rectangle_L rectangleLightShape;
	ProvidedMesh* tube;
	vec4 lightColor;
	vec3 x{ 0, 0, 0 };
	Vector* N;
	Vector* V;
	Vector* L;
	Vector* Na;
	Vector* R;
	float offset;
};