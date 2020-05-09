#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include "../ShadowMapping/ShadowMappingScene.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class PointShadowScene : public Scene {
public:
	PointShadowScene() :Scene("Point Shadow") {
        addShaderFromFile("phong", "../ShadowMapping/shaders/phong.vert");
        addShaderFromFile("phong", "../ShadowMapping/shaders/phong.frag");
	}

	void init() override {
        initDefaultCamera();
        activeCamera().collisionTestOff();
        activeCamera().setMode(Camera::FIRST_PERSON);
        loadPlankMaterial();
        lightPos = vec3(0);
        activeCamera().setPosition({ 0, 0, 3 });
        pointShadowMap = OminiDirectionalShadowMap{ 5, lightPos, 1024, 1024 };
        lightObj = new Sphere(0.3, 20, 20, WHITE);
        createCubeForPointShadow();
	}

	void display() override {
      //  captureScene();
        pointShadowMap.update(lightPos);
        pointShadowMap.capture([&] {
            shade(&cube);
        });
        shader("phong").use();

        send("lightPos", lightPos);
        send(activeCamera());

        send("shadowOn", true);
        send("camPos", activeCamera().getPosition());
        glBindTextureUnit(0, plankMaterail.albedo());
        glBindTextureUnit(1, plankMaterail.albedo());
        glBindTextureUnit(2, plankMaterail.ambientOcclusion());
        glBindTextureUnit(3, plankMaterail.normal());
        send(pointShadowMap);
        shade(&cube);
        shader("phong").unUse();

        shader("flat")([&] {
            auto model = translate(mat4{ 1 }, lightPos);
            send(activeCamera(), model);
            shade(lightObj);
        });
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

        woodTexture = plankMaterail.albedo();
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

        cube = Cube(2, RED, models, false);
    }

	void update(float t) override {
        lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;
	}

private:
	unsigned int depthMapFBO;
	unsigned int depthCubemap;
    unsigned int cubeVAO;
    unsigned int cubeVBO;
    unsigned int woodTexture;
    vec3 lightPos;
    float near_plane = 1.0f;
    float far_plane = 25.0f;
    bool shadows = true;
    pbr::TextureMaterial plankMaterail;
    OminiDirectionalShadowMap pointShadowMap;
    Sphere* lightObj;
    Cube cube;
};