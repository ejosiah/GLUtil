#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/util/SphericalCoord.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"


using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

const int NORMAL_DISTRIBUTION_FUNC = 1 << 0;
const int GEOMETRY_FUNC = 1 << 1;
const int FRENEL = 1 << 2;
const int ALL = NORMAL_DISTRIBUTION_FUNC | GEOMETRY_FUNC | FRENEL;

enum class LightType {
	BSDF, RENDER_EQU, PHONG, BLING_PHONG
};


class PbrScene : public Scene {
public:
	PbrScene() :Scene("PBR Scene", 1280, 960) {
		useImplictShaderLoad(true);
		addShader("spec_brdf", GL_FRAGMENT_SHADER, identity_frag_shader);
		bitfield = NORMAL_DISTRIBUTION_FUNC;
		currentLightType = LightType::BSDF;
	}

	void init() override {
		//initDefaultCamera();
		//auto& cam = activeCamera();
		ScalarMat =  pbr::ScalarMaterial{ "Scalar", vec3{0, 0, 1}, 0.5f, 0.5f };
		//tMat = pbr::TextureMaterial{ vec3{0, 1, 0}, 0.5f, 0.5f, 256, 256 };

		gold = pbr::TextureMaterial{
			"Gold",
			pbr::Material::Albedo{"textures\\materials\\gold\\albedo.png"},
			pbr::Material::Normal{"textures\\materials\\gold\\normal.png"},
			pbr::Material::Metalness{"textures\\materials\\gold\\metallic.png"},
			pbr::Material::Roughness{"textures\\materials\\gold\\roughness.png"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\gold\\ao.png"}
		};

		rustedMetal = pbr::TextureMaterial{
			"Rusted Metal",
			pbr::Material::Albedo{"textures\\materials\\rusted_iron\\albedo.png"},
			pbr::Material::Normal{"textures\\materials\\rusted_iron\\normal.png"},
			pbr::Material::Metalness{"textures\\materials\\rusted_iron\\metallic.png"},
			pbr::Material::Roughness{"textures\\materials\\rusted_iron\\roughness.png"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\gold\\ao.png"}
		};

		plastic = pbr::TextureMaterial{
			"Plastic",
			pbr::Material::Albedo{"textures\\materials\\plastic\\albedo.png"},
			pbr::Material::Normal{"textures\\materials\\plastic\\normal.png"},
			pbr::Material::Metalness{"textures\\materials\\plastic\\metallic.png"},
			pbr::Material::Roughness{"textures\\materials\\plastic\\roughness.png"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\plastic\\ao.png"}
		};


		grass = pbr::TextureMaterial{
			"Grass",
			pbr::Material::Albedo{"textures\\materials\\grass\\albedo.png"},
			pbr::Material::Normal{"textures\\materials\\grass\\normal.png"},
			pbr::Material::Metalness{"textures\\materials\\grass\\metallic.png"},
			pbr::Material::Roughness{"textures\\materials\\grass\\roughness.png"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\grass\\ao.png"}
		};
		

		wall = pbr::TextureMaterial{
			"Wall",
			pbr::Material::Albedo{"textures\\materials\\wall\\albedo.png"},
			pbr::Material::Normal{"textures\\materials\\wall\\normal.png"},
			pbr::Material::Metalness{"textures\\materials\\wall\\metallic.png"},
			pbr::Material::Roughness{"textures\\materials\\wall\\roughness.png"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\wall\\ao.png"}
		};

		weavedMetal = pbr::TextureMaterial{
			"Weaved Metal",
			pbr::Material::Albedo{"textures\\materials\\weavedMetal\\albedo.jpg"},
			pbr::Material::Normal{"textures\\materials\\weavedMetal\\normal.jpg"},
			pbr::Material::Metalness{"textures\\materials\\weavedMetal\\metallic.jpg"},
			pbr::Material::Roughness{"textures\\materials\\weavedMetal\\gloss.jpg"},
			pbr::Material::AmbientOcculusion{"textures\\materials\\weavedMetal\\ao.jpg"},
			true,
			true
		};
		chrome = pbr::TextureMaterial{
				"Chrome",
				pbr::Material::Albedo{"textures\\materials\\chrome\\albedo.jpg"},
				pbr::Material::Normal{"textures\\materials\\chrome\\normal.jpg"},
				pbr::Material::Metalness{"textures\\materials\\chrome\\metallic.jpg"},
				pbr::Material::Roughness{"textures\\materials\\chrome\\gloss.jpg"},
				pbr::Material::AmbientOcculusion{"textures\\materials\\chrome\\ao.jpg"},
				true,
				true
		};

		materials.push_back(&gold);
		materials.push_back(&rustedMetal);
		materials.push_back(&plastic);
		materials.push_back(&grass);
		materials.push_back(&wall);
		materials.push_back(&weavedMetal);
		materials.push_back(&chrome);

		currentMaterial = materials[nextMaterial];

		cam.view = lookAt(radius, vec3(0), color());
		sCoord = cs::SphericalCoord(radius);
		
		lightModel.colorMaterial = false;
		lightModel.twoSided = true;

		sphere = new Sphere(0.3f);
		cube = new Cube(0.7);
		objectToWorld = translate(mat4(1), { 0, 2, 0 });
		worldToObject = inverse(objectToWorld);
		model = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 1);

		//for (int i = 0; i < model->numMeshes(); i++) {
		//	model->material(i).diffuse = RED;
		//}

		font = Font::Arial(15, Font::BOLD);
		light[0].on;
		light[0].position = { 0, 0, 1 , 0};
		pbr::Scene scene;
		scene.eyes = vec3(sCoord);
		scene.lights[0].position = { 0, 0, 3 };
		scene.lights[0].Intensity = vec3(23.47, 21.31, 20.79);
		scene.lights[1].position = { 0, 3, 0 };
		scene.lights[1].Intensity = vec3(23.47, 21.31, 20.79);
		scene.lights[2].position = { 3, 0, 0 };
		scene.lights[2].Intensity = vec3(23.47, 21.31, 20.79);
		scene.lights[3].position = { 0, 0, -3 };
		scene.lights[3].Intensity = vec3(23.47, 21.31, 20.79);
		scene.lights[4].position = { 0, -3, 0 };
		scene.lights[4].Intensity = vec3(23.47, 21.31, 20.79);
		scene.lights[5].position = { -3, 0, 0 };
		scene.lights[5].Intensity = vec3(23.47, 21.31, 20.79);
		numLights = 6;
		ssboScene = StorageBufferObj<pbr::Scene>{ scene, 0};

		glDisable(GL_CULL_FACE);
	}

	void display() override {
		ssboScene.get().eyes = vec3(sCoord);

		switch (currentLightType) {
		case LightType::BSDF:
			displayBSDF();
			break;
		case LightType::RENDER_EQU:
			displayRenderEqu();
			break;
		case LightType::PHONG:
			break;
		case LightType::BLING_PHONG:
			break;
		}

		displayText();
	}

	void displayBSDF() {
		shader("spec_brdf")([&]() {
			send("bitfield", bitfield);
			send(ScalarMat);
			send("lightPos", light[0].position.xyz);
			send("viewPos", vec3(sCoord));
			send(cam);
			shade(model);
			});
	}

	void displayRenderEqu() {
		shader("pbr")([&](Shader& s) {
			//	send(material);
			send(*currentMaterial);
			//send("albedo", vec3(1, 1, 1));
			//send("metalness", metalness);
			//send("roughness", roughness);
			//send("glossiness", true);
			//send("invertBlack", true);
			send("directional", true);
			send("numLights", numLights);
			send("useNormalMapping", useNormalMapping);
			//send("lightPos", ssboScene.get().lights[0].position);
			//send("intensity", ssboScene.get().lights[0].Intensity);
		//	send("eyes", eyes);
			send(ssboScene);
			send(cam);
			shade(model);
			//	shade(cube);
			//	shade(sphere);
				//model->draw(s, 1);
				//model->draw(s, 2);

				//send(gold);
				//model->draw(s, 3);
				//model->draw(s, 4);
			});
	}

	void displayPhong() {

	}

	void displayBlingPhong() {

	}

	void displayText() {
		ss.str("");
		ss.clear();

		auto typeName = currentLightTypeName();
		ss << "LightTypes:\t";
		ss << "Phong: 6\t";
		ss << "Bling Phong: 7\t";
		ss << "Cook-Torrance BRDF: 8\t";
		ss << "Physically Based Rendering: 9\t\n";
		ss << "CurrentType: " << typeName;

		font->render(ss.str(), 20, 20);

		ss.str("");
		ss.clear();

		switch (currentLightType) {
		case LightType::BSDF:
			displayBSDFText();
			break;
		case LightType::RENDER_EQU:
			displayRenderEquText();
			break;
		case LightType::PHONG:
			break;
		case LightType::BLING_PHONG:
			break;
		}
		ss.str("");
		ss.clear();

		ss << "ViewPos: " << vec3(sCoord) << "\n";
		ss << "RightPos: " << light[0].position.xyz;
		font->render(ss.str(), _width - 300, 20);
	}

	string currentLightTypeName() {
		switch (currentLightType) {
		case LightType::BSDF:
			return "Cook-Torrance BRDF";
		case LightType::RENDER_EQU:
			return "Physically Based Rendering";
		case LightType::PHONG:
			return "Phong";
		case LightType::BLING_PHONG:
			return "Bling Phong";
		}
	}

	void displayBSDFText() {
		ss << "Normal distribution function: 0\t";
		ss << "Geomertry function: 1\t";
		ss << "Frenel: 2\n";

		if ((bitfield & NORMAL_DISTRIBUTION_FUNC) == NORMAL_DISTRIBUTION_FUNC) {
			ss << "Selection: Normal distribution function\n";
		}
		if ((bitfield & GEOMETRY_FUNC) == GEOMETRY_FUNC) {
			ss << "Selection: Geometry function\n";
		}
		if ((bitfield & FRENEL) == FRENEL) {
			ss << "Selection: Frenel\n";
		}
		if ((bitfield & ALL) == ALL) {
			ss << "Selection: Normal distribution function + Geometry function + Frenel\n";
		}

		ss << "Albedo: " << ScalarMat.albedo;
		ss << "\nRoughness: " << setw(2) << ScalarMat.roughness;
		ss << "\nMetalness: " << ScalarMat.metalness;
		font->render(ss.str(), 20, 60);
	}

	void displayRenderEquText() {
		ss << "Material: " << currentMaterial->name();
		ss << "\nNormalMapping: " << (useNormalMapping ? "On" : "Off");
		font->render(ss.str(), 20, 60);
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
			case '6':
				currentLightType = LightType::PHONG;
				return;
			case '7':
				currentLightType = LightType::BLING_PHONG;
				return;
			case '8':
				currentLightType = LightType::BSDF;
				return;
			case '9':
				currentLightType = LightType::RENDER_EQU;
				return;
			}

			switch (currentLightType) {
			case LightType::BSDF:
				processBSDFInput(key);
				break;
			case LightType::RENDER_EQU:
				processRenderEqu(key);
				break;
			case LightType::PHONG:
				break;
			case LightType::BLING_PHONG:
				break;
			}
		}
	}

	void processBSDFInput(const Key& key) {
		float prevStep = step;
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
				ScalarMat.roughness += 0.05;
				break;
			case 'R':
				ScalarMat.roughness -= 0.05;
				break;
			case 'm':
				ScalarMat.metalness += 0.05;
				break;
			case 'M':
				ScalarMat.metalness -= 0.05;
				break;
			case 'a':
				step += 0.01;
				break;
			case 'A':
				step -= 0.01;
				break;
			}
			ScalarMat.roughness = glm::clamp(ScalarMat.roughness, 0.1f, 0.9f);
			ScalarMat.metalness = glm::clamp(ScalarMat.metalness, 0.0f, 1.0f);
			if (step != prevStep) {
				step = glm::clamp(step, 0.0f, 1.0f);
				ScalarMat.albedo = color();
			}
		}
	}

	vec3 color() {
		float r = smoothstep(0.6f, 0.8f, step);
		float g = smoothstep(0.0f, 0.4f, step) - smoothstep(0.8f, 1.0f, step);;
		float b = 1 - smoothstep(0.4f, 0.6f, step);
		return { r, g, b};
	}

	void processRenderEqu(const Key& key) {
		switch (key.value()) {
		case 'n':
			useNormalMapping = !useNormalMapping;
			break;
		case 'm':
			nextMaterial++;
			nextMaterial %= materials.size();
			currentMaterial = materials[nextMaterial];
			break;
		}
	}

private:
	Sphere* sphere;
	Cube* cube;
	Model* model;
	mat4 objectToWorld;
	mat4 worldToObject;
	LightModel lightModel;
	vec3 radius = { 0, 0, 1 };
	cs::SphericalCoord sCoord;
	Font* font;
	pbr::ScalarMaterial ScalarMat;
	pbr::TextureMaterial rustedMetal;
	pbr::TextureMaterial plastic;
	pbr::TextureMaterial gold;
	pbr::TextureMaterial weavedMetal;
	pbr::TextureMaterial wall;
	pbr::TextureMaterial chrome;
	pbr::TextureMaterial grass;
	pbr::Material* currentMaterial;
	vector<pbr::Material*> materials;
	int nextMaterial = 0;
	int bitfield;
	float step = 0;
	stringstream ss;
	int numLights;
	StorageBufferObj<pbr::Scene> ssboScene;
	LightType currentLightType;
	bool useNormalMapping;
};