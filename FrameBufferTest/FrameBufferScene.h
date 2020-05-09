#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include <iterator>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class FrameBufferScene : public Scene {
public:
	FrameBufferScene() :Scene("Framebuffer scene") {
		useImplictShaderLoad(true);
		addShader("fbo", GL_VERTEX_SHADER, identity_vert_shader);
		addShader("fbo", GL_FRAGMENT_SHADER, texture_frag_shader);

		addShader("screen", GL_VERTEX_SHADER, identity_vert_shader);
		addShader("brdf", GL_VERTEX_SHADER, brdf_vert_shader);
		addShader("brdf", GL_FRAGMENT_SHADER, bsdf_frag_shader);

		addShader("screen0", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen0", GL_FRAGMENT_SHADER, screen_frag_shader);
	}

	void init() override {
		cam.view = lookAt({ 0 , 0, 3 }, vec3(0), { 0, 1, 0 });
		cube = new Cube(1);

		cam1.view = mat4{ 1 };
		cam1.model = mat4{ 1 };
		cam1.projection = ortho(-1, 1, -1, 1);

		buildPlane();
		buildQuad();
		loadTextures();
		buildFrameBuffer();
	//	loadFrameBuffer();
		setBackGroundColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		defer([&]() {
			brdf_lut = pbr::generate_brdf_lookup_table(0);
		});
	}

	void buildPlane() {
		Mesh mesh;
		mesh.positions.emplace_back(5.0f, -0.5f, 5.0f);
		mesh.positions.emplace_back(-5.0f, -0.5f, 5.0f);
		mesh.positions.emplace_back(-5.0f, -0.5f, -5.0f);
		mesh.positions.emplace_back(5.0f, -0.5f, 5.0f);
		mesh.positions.emplace_back(-5.0f, -0.5f, -5.0f);
		mesh.positions.emplace_back(5.0f, -0.5f, -5.0f);

		mesh.uvs[0].emplace_back(2.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 2.0f);
		mesh.uvs[0].emplace_back(2.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 2.0f);
		mesh.uvs[0].emplace_back(2.0f, 2.0f);

		mesh.primitiveType = GL_TRIANGLES;
		plane = new ProvidedMesh{ mesh };
	}

	void buildQuad() {
		Mesh mesh;

		mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
		mesh.positions.emplace_back(-1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
		mesh.positions.emplace_back(1.0f, 1.0f, 0.0f);

		mesh.uvs[0].emplace_back(0.0f, 1.0f);
		mesh.uvs[0].emplace_back(0.0f, 0.0f);
		mesh.uvs[0].emplace_back(1.0f, 0.0f);
		mesh.uvs[0].emplace_back(0.0f, 1.0f);
		mesh.uvs[0].emplace_back(1.0f, 0.0f);
		mesh.uvs[0].emplace_back(1.0f, 1.0f);

		quad = new ProvidedMesh{ mesh };
	}

	void loadTextures() {
		marble = new Texture2D("textures\\marble.jpg", 0);
		metal = new Texture2D("textures\\metal.png", 0);
	}

	void buildFrameBuffer() {
		FrameBuffer::Config config{ _width, _height };
		config.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		fbo = FrameBuffer{ config };
	}

	void display() override{
		fbo.use([&]() {
			glEnable(GL_DEPTH_TEST);
			shader("fbo")([&](Shader& s) {
				cam.model = translate(mat4(1), vec3(-1.0f, 0.0f, -1.0f));
				send(cam);
				send(metal);
				shade(cube);
				cam.model = translate(mat4(1), { 2.0f, 0.0, 0.0 });
				send(marble);
				send(cam);
				shade(plane);
			});
			glDisable(GL_DEPTH_TEST);
		});

		shader("screen0")([&](Shader& s) {
			glBindTextureUnit(0, fbo.texture());
			//send(brdf_lut);
		//	sendArray("kernel", identity, 9);
			//send(cam1);
			shade(quad);
		});
	}

	void resized() {
		cam.projection = perspective(pi<float>() / 3, aspectRatio, 0.1f, 100.f);
	}


private:
	FrameBuffer fbo;
	ProvidedMesh* plane;
	ProvidedMesh* quad;
	Texture2D* marble;
	Texture2D* metal;
	Cube* cube;
	GlmCam cam1;
	float blur[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	float sharpen[9] = {
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	};
	float edgeDetect[9] = {
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	};

	float gradNorth[9] = {
		1, 2, 1,
		0,	0,	0,
		-1, -2, -1
	};
	float _1_over_9 = 1.0 / 9;
	float mean[9] = {
		_1_over_9, _1_over_9, _1_over_9,
		_1_over_9, _1_over_9, _1_over_9,
		_1_over_9, _1_over_9, _1_over_9
	};
	float laplacian[9]{
		0, -1, 0,
		-1, 4, -1,
		0, -1, 0
	};

	float kernel[9]{
		0, 1, 0,
		0, 0, 0,
		0, 0, 0
	};

	float identity[9]{
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	};
	Texture2D* brdf_lut;
};