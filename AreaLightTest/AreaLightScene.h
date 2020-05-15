#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/LightProbe.h"


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
		cube = Cube{ 1.0f, {WHITE, GREEN, WHITE, RED, WHITE, WHITE}, {model}, false };

		orb = new Model("C:\\Users\\" + username + "\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 0.5);
		auto midpoint = (orb->bound->max() - orb->bound->min()) * 0.5f;
		offset = dot(midpoint, { 0, 1, 0 });
		activeCamera().setMode(Camera::Mode::FIRST_PERSON);
		activeCamera().lookAt({ 0, 2, 3 }, vec3(0), { 0, 1, 0 });
		auto& camm = activeCamera();


		pos = new Sphere(0.1, 10, 10, RED);
		initRectangleLight();
		initSphereLight();
		initDiskLight();
		initTubeLight();
		//initVectors();
	}

	void initRectangleLight() {
		lights[0].type = AreaSphere;
		lights[0].unit = Luminance;
		lights[0].value = 700;
		lights[0].color = lightColor.xyz;
		lights[0].radius = 0.5f;
		lights[0].shape = createRectangle();

		lights[0].localToWorld = translate(mat4(1), { 4, 2,  0 });
		lights[0].localToWorld = rotate(lights[0].localToWorld, glm::radians(-90.0f), { 0, 0, 1 });
		rectangleLightShape.width = 2.0f;
		rectangleLightShape.height = 2.0f;
		lights[0].shapeId = 0 | RECTANGLE_SHAPE;
	}

	void initSphereLight() {
		sphereLightShape.center = vec3(0);
		sphereLightShape.radius = 0.5f;
		lights[1].type = AreaSphere;
		lights[1].unit = Luminance;
		lights[1].value = 700;
		lights[1].color = lightColor.xyz;
		lights[1].radius = 0.5f;
		lights[1].shape = new Sphere(sphereLightShape.radius, 50, 50, lightColor);

		lights[1].localToWorld = translate(mat4(1), { 0, 2,  0 });
		rectangleLightShape.width = 2.0f;
		rectangleLightShape.height = 2.0f;
		lights[1].shapeId = 0 | SPHERE_SHAPE;
	}

	void initDiskLight() {
		diskLightShape.radius = 1.0f;
		diskLightShape.center = vec3(0.f);
		lights[2].type = AreaSphere;
		lights[2].unit = Luminance;
		lights[2].value = 700;
		lights[2].color = lightColor.xyz;
		lights[2].radius = 0.5f;
		lights[2].shape = createDisk(diskLightShape);

		lights[2].localToWorld = translate(mat4(1), { -4, 2,  0 });
		lights[2].localToWorld = rotate(lights[2].localToWorld, glm::radians(90.0f), { 0, 0, 1 });

		lights[2].shapeId = 0 | DISK_SHAPE;
	}

	void initTubeLight() {
		tubeLightShape.center = vec3(0);
		tubeLightShape.height = 2;
		tubeLightShape.radius = 0.2;
		lights[3].type = AreaSphere;
		lights[3].unit = Luminance;
		lights[3].value = 700;
		lights[3].color = lightColor.xyz;
		lights[3].radius = 0.5f;
		lights[3].shape = createTube(tubeLightShape);

		lights[3].localToWorld = translate(mat4(1), { 2.5, tubeLightShape.radius,  0 });
	//	light.localToWorld = rotate(light.localToWorld, glm::radians(90.0f), { 0, 1, 0 });
		lights[3].shapeId = 0 | TUBE_SHAPE;
	}

	//void initVectors() {
	//	vec3 eyes = activeCamera().getPosition();
	//	vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;
	//	vec3 l = lightPos - x;
	//	vec3 v = normalize(activeCamera().getPosition() - x);
	//	vec3 n = { 0, 1, 0 };
	//	vec3 r = reflect(v, n);
	//	mat3 lightToWorld3 = mat3(transpose(inverse(light.localToWorld)));
	//	vec3 na = normalize(lightToWorld3 * light.normal);

	//	L = new Vector{ l, x, 1.0f, BLUE };
	//	V = new Vector{ v, x, 1.0f, GREEN };
	//	N = new Vector{ n, x, 1.0f, RED};
	//	Na = new Vector{ na, lightPos, 1.0f, RED };
	//	R = new Vector{ r, x , 1.0, YELLOW };
	//}

	Shape* createDisk(Disk_L& disk) {
		Mesh mesh;
		float segments = 100;
		float dt = two_pi<float>() / segments;
		float PI = pi<float>();
		float r = disk.radius;
		for (float t = -PI; t <= PI; t += dt) {
			mesh.positions.push_back(vec3{ r * cos(t), 0, r * sin(t) });
			mesh.colors.push_back(lightColor);
		}
		mesh.primitiveType = GL_TRIANGLE_FAN;
		return new ProvidedMesh(mesh);
	}

	Shape* createRectangle() {
		Mesh mesh;
		mesh.positions.emplace_back(1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, -1.0f);
		mesh.positions.emplace_back(1.0f, 0.0f, 1.0f);
		mesh.positions.emplace_back(-1.0f, 0.0f, -1.0f);
		mesh.positions.emplace_back(1.0f, -0.0f, -1.0f);
		mesh.colors = vector<vec4>(6, lightColor);
		mesh.primitiveType = GL_TRIANGLES;

		return new ProvidedMesh(mesh);
	}

	Shape* createTube(Tube_L& tube) {
		vector<Mesh> meshes;
		auto cylinder = new Cylinder(tube.radius, tube.height);
		auto cap0 = new Hemisphere(tube.radius);
		Mesh mesh = cap0->getMeshes().at(0);
		Mesh frontCap = mesh;
		frontCap.name = "front_cap";
		mat4 model = translate(mat4{ 1 }, { 0, 0, tube.height * 0.5f });
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
		model = translate(mat4{ 1 }, { 0.0f, 0.0f, tube.height * 0.5f });

		std::transform(body.positions.begin(), body.positions.end(), body.positions.begin(), [&](auto p) {
			return vec3((model * vec4(p, 1.0f)));
		});

		vector<vec4> colors{ body.positions.size(), lightColor };
		frontCap.colors = vector<vec4>{ colors };
		body.colors = colors;
		backCap.colors = vector<vec4>{ colors };




		meshes.push_back(frontCap);
		meshes.push_back(body);
		meshes.push_back(backCap);
		delete cylinder;

		return new ProvidedMesh(meshes);
	}

	void display() override {
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader("area_light")([&]() {
			send(activeCamera());
			renderScene(activeCamera().getPosition());
		});

		shader("flat")([&]() {
			glDisable(GL_DEPTH_TEST);
			for (int i = 0; i < numLights; i++) {
				send(activeCamera(), lights[i].localToWorld);
				shade(lights[i].shape);
			}
			glEnable(GL_DEPTH_TEST);
			//send(activeCamera());
			//shade(pos);
			//shade(L);
			//shade(V);
			//shade(N);
			//shade(Na);
			//shade(R);
		});
	}

	void renderScene(vec3 eyes) {
		shade(&cube);
		mat4 model = translate(mat4(1), { 0, offset, 0 });
		model = rotate(model, half_pi<float>(), { 0, 1, 0 });
		send("M", model);
		shade(orb);
		send("roughness", 0.0f);
		send("eyes", eyes);
		for (int i = 0; i < numLights; i++) {
			_send(lights[i], i);
		}
		send("numLights", numLights);

	}

	void _send(Light& light, int i) {
		send("lights[" + to_string(i) +  "].type", light.type);
		send("lights[" + to_string(i) + "].unit", light.unit);
		send("lights[" + to_string(i) + "].position", light.position);
		send("lights[" + to_string(i) + "].value", light.value);
		send("lights[" + to_string(i) + "].color", light.color);
		send("lights[" + to_string(i) + "].radius", light.radius);
		send("lights[" + to_string(i) + "].localToWorld", light.localToWorld);
		send("lights[" + to_string(i) + "].normal", light.normal);
		send("lights[" + to_string(i) + "].shapeId", light.shapeId);
		
		send("rectangleLights[0].width", rectangleLightShape.width);
		send("rectangleLights[0].height", rectangleLightShape.height);
		
		send("SphereLights[0].radius", sphereLightShape.radius);
		send("SphereLights[0].center", sphereLightShape.center);

		send("diskLights[0].radius", diskLightShape.radius);
		send("diskLights[0].center", diskLightShape.center);

		send("tubeLights[0].radius", tubeLightShape.radius);
		send("tubeLights[0].center", tubeLightShape.center);
		send("tubeLights[0].height", tubeLightShape.height);
	}

	void update(float t) {
		//vec3 v = normalize(activeCamera().getPosition() - x);
		//vec3 r = -reflect(v, { 0, 1, 0 });
		//V->update(v);
		//R->update(r);
	}

	void resized() override {
		//activeCamera().perspective(65.0f, aspectRatio, 0.1f, 100.0f);
	}

private:
	Cube cube;
	int numLights = 1;
	Model* orb;
	Light lights[5];
	Sphere* pos;
	Rectangle_L rectangleLightShape;
	Sphere_L sphereLightShape;
	Disk_L diskLightShape;
	Tube_L tubeLightShape;
	ProvidedMesh* tube;
	vec4 lightColor;
	vec3 x{ 0, 0, 0 };
	Vector* N;
	Vector* V;
	Vector* L;
	Vector* Na;
	Vector* R;
	Probe probe;
	float offset;
};