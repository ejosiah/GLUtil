#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/util/SphericalCoord.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

const int NORMAL_DISTRIBUTION_FUNC = 1 << 0;
const int GEOMETRY_FUNC = 1 << 1;
const int FRENEL = 1 << 2;
const int ALL = NORMAL_DISTRIBUTION_FUNC | GEOMETRY_FUNC | FRENEL;


class PbrScene : public Scene {
public:
	PbrScene() :Scene("PBR Scene", 1280, 960) {
		useImplictShaderLoad(true);
		addShader("spec_brdf", GL_FRAGMENT_SHADER, identity_frag_shader);
		bitfield = NORMAL_DISTRIBUTION_FUNC;
	}

	void init() override {
		//initDefaultCamera();
		//auto& cam = activeCamera();
		cam.view = lookAt(radius, vec3(0), { 0, 1, 0 });
		sCoord = cs::SphericalCoord(radius);
		
		lightModel.colorMaterial = false;
		lightModel.twoSided = true;

		sphere = new Sphere(2.0f);
		objectToWorld = translate(mat4(1), { 0, 2, 0 });
		worldToObject = inverse(objectToWorld);
		model = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj");

		//for (int i = 0; i < model->numMeshes(); i++) {
		//	model->material(i).diffuse = RED;
		//}

		font = Font::Arial(15);

		glDisable(GL_CULL_FACE);
	}

	void display() override {
		light[0].on;
		//light[0].position = vec4(activeCamera().getPosition(), 1);

		//shader("flat")([&](Shader& s) {
		//	send("gammaCorrect", true);
		//	send(light[0]);
		//	//send(activeCamera(), objectToWorld);
		//	send(cam);
		//	send(lightModel);
		//	shade(model);
		//});

		shader("spec_brdf")([&]() {
			send("bitfield", bitfield);
			send("alpha", roughness);
			send("metalness", metalness);
			send("lightPos", light[0].position.xyz);
			send("viewPos", vec3(sCoord));
			send(cam);
			shade(model);
		});


		displayText();
	}

	void displayText() {
		ss.str("");
		ss.clear();

		ss << "Normal distribution function: 0\t";
		ss << "Geomertry function: 1\t";
		ss << "Frenel: 2\n";

		if ((bitfield & NORMAL_DISTRIBUTION_FUNC) == NORMAL_DISTRIBUTION_FUNC) {
			ss << "selection: Normal distribution function\n";
		}
		if ((bitfield & GEOMETRY_FUNC) == GEOMETRY_FUNC) {
			ss << "selection: Geometry function\n";
		}
		if ((bitfield & FRENEL) == FRENEL) {
			ss << "selection: Frenel\n";
		}
		if ((bitfield & ALL) == ALL) {
			ss << "selection: Normal distribution function + Geometry function + Frenel\n";
		}

		ss << "roughness: " << setw(2) << roughness;
		ss << "\nmetalness: " << metalness;
		font->render(ss.str(), 20, 20);

		ss.str("");
		ss.clear();

		ss << "viewPos: " << vec3(sCoord) << "\n";
		ss << "lightPos: " << light[0].position.xyz;
		font->render(ss.str(), _width - 200, 20);

	}

	virtual void update(float dt) override {
		Mouse& mouse = Mouse::get();
		sCoord.updateTheta(mouse.relativePos.y * 0.01f);
		sCoord.updatePhi(mouse.relativePos.x * 0.01f);
		mouse.recenter();
		auto pos = vec3(sCoord);
		cam.view = lookAt(vec3(sCoord), vec3(0), { 0, 1, 0 });
	}

	void resized() override {
		//activeCamera().perspective(60.0f, float(_width) / _height, 0.01, 1000.0f);
		cam.projection = perspective(pi<float>()/3, aspectRatio, 0.01f, 1000.0f);
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case '0':
				bitfield = NORMAL_DISTRIBUTION_FUNC;
				break;
			case '1':
				bitfield = GEOMETRY_FUNC;
				break;
			case '2':
				bitfield = FRENEL;
				break;
			case '3':
				bitfield = ALL;
				break;
			case 'r':
				roughness += 0.05;
				break;
			case 'R':
				roughness -= 0.05;
				break;
			case 'm':
				metalness += 0.05;
				break;
			case 'M':
				metalness -= 0.05;
				break;
			}
			roughness = glm::clamp(roughness, 0.1f, 0.9f);
			metalness = glm::clamp(metalness, 0.0f, 1.0f);
		}
	}

private:
	Sphere* sphere = nullptr;
	Model* model;
	mat4 objectToWorld;
	mat4 worldToObject;
	LightModel lightModel;
	vec3 radius = { 0, 0, 1 };
	cs::SphericalCoord sCoord;
	Font* font;
	float roughness = 0.5f;
	float metalness = 0.5f;
	int bitfield;
	stringstream ss;
};