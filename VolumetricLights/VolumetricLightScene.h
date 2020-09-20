#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"
#include "../GLUtil/include/ncl/sampling/sampling.h"
#include "../GLUtil/include/ncl/gl/ShadowMap.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;


class VolumetricLightScene : public Scene {
public:
	VolumetricLightScene() :Scene("Volumetric Lights") {
		addShader("debug_shadow_map", GL_VERTEX_SHADER, point_shadow_map_render_vert_shader);
		addShader("debug_shadow_map", GL_FRAGMENT_SHADER, point_shadow_map_render_frag_shader);
	}

	void init() override {
		initDefaultCamera();
		activeCamera().setPosition({ 0, 0, 0 });
		activeCamera().collisionTestOff();
		sphere = Sphere{ 1 };
		sphere.defautMaterial(false);
		sphere.cullBackFaceOff();

		sphereBounds = Sphere{ 2 };
		sphereBounds.defautMaterial(false);
		sphereBounds.cullBackFaceOff();

		cam.view = lookAt({ 0, 0, 5 }, vec3(0), { 0, 1, 0 });

		auto points = sampling::hammersleySphere(numPoints);

		vector<vec4> holes;
		Mesh mesh;
		mesh.primitiveType = GL_POINTS;

		for (auto p : points) {
			auto num_p = normalize(p);
			holes.push_back(vec4(p, 0.2));
			mesh.positions.push_back(p);
			mesh.colors.push_back(GREEN);
		}
		holesSsbo = StorageBufferObj<vec4>{ holes, 0 };
		pMesh = ProvidedMesh{ mesh };
		vec3 p = sampling::pointInSphere(0.5, 0.8);
		float r = length(p);
		logger.info("radius: " + to_string(r));
		glPointSize(5);

		shadowMap = OminiDirectionalShadowMap{ 5, lightPos, 1024, 1024};
		cube = Cube{ 1 };
	}

	void display() override {
		shadowMap.update(lightPos);
		shadowMap.capture([&] {
			shader("holes")([&] {
				holesSsbo.sendToGPU();
				send("num_holes", numPoints);
			//	send(activeCamera());
				shade(sphere);
			});

			//shader("flat")([&] {
			//	send(activeCamera());
			//	shade(sphereBounds);
			//});
			});
		
		

		//shader("holes")([&] {
		//	glBindTextureUnit(5, shadowMap.texture());
		//	holesSsbo.sendToGPU();
		//	send("num_holes", numPoints);
		//	send(activeCamera());
		//	shade(sphere);
		//});

		shader("debug_shadow_map")([&] {
			glDepthFunc(GL_LEQUAL);
			glBindTextureUnit(0, shadowMap.texture());
			send(activeCamera());
			shade(cube);
			glDepthFunc(GL_LESS);
		});

	}

	void resized() override {
		cam.projection = perspective(radians(60.0f), aspectRatio, 0.1f, 10.0f);
	}

private:
	Model* sponza;
	int numPoints = 10;
	Sphere sphere;
	Sphere sphereBounds;
	StorageBufferObj<vec4> holesSsbo;
	ProvidedMesh pMesh;
	Logger logger = Logger::get("VL");
	OminiDirectionalShadowMap shadowMap;
	vec3 lightPos = vec3(0);
	Cube cube;;
};