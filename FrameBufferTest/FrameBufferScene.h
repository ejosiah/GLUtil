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

		//addShader("screen", GL_VERTEX_SHADER, identity_vert_shader);
		//addShader("brdf", GL_VERTEX_SHADER, brdf_vert_shader);
		//addShader("brdf", GL_FRAGMENT_SHADER, bsdf_frag_shader);

		addShader("screen0", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen0", GL_FRAGMENT_SHADER, screen_frag_shader);

		addShader("bmp_to_nmp", GL_VERTEX_SHADER, screen_vert_shader);
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
		bmp = new Texture2D(path, 0);
	}

	void buildFrameBuffer() {
		auto  config = FrameBuffer::defaultConfig( _width, _height);
		config.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

	//	Texture2D(void* data, GLuint width, GLuint height, std::string name = "", GLuint id = 0, GLuint iFormat = GL_RGBA8, GLuint format = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, glm::vec2 wrap = glm::vec2{ GL_CLAMP_TO_EDGE }, glm::vec2 minMagfilter = glm::vec2{ GL_LINEAR })
		scratchTexel = new Texture2D{ nullptr, (unsigned)_width, (unsigned)_height };

		fbo = FrameBuffer{ config };
	}

	void display() override{
		fbo.use(scratchTexel, [&]() {
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

		shader("bmp_to_nmp")([&](Shader& s) {
		//	glBindTextureUnit(0, fbo.texture());
		//	glBindTextureUnit(0, scratchTexel->buffer());
			glBindTextureUnit(0, bmp->buffer());
			//send(brdf_lut);
			sendArray("kernel", identity, 9);
			send("flip", flip);
			send(cam1);
			shade(quad);
		});
	}

	void resized() {
		cam.projection = perspective(pi<float>() / 3, aspectRatio, 0.1f, 100.f);
	}

	void processInput(const Key& key) override {
		if (key.value() == 'o' && key.pressed()) {
			flip = !flip;
		}
	}

private:
	FrameBuffer fbo;
	Texture2D* scratchTexel;
	ProvidedMesh* plane;
	ProvidedMesh* quad;
	Texture2D* marble;
	Texture2D* metal;
	Cube* cube;
	GlmCam cam1;
	Texture2D* bmp;
//	string path = "C:\\Users\\Josiah\\OneDrive\\media\\models\\cannon\\Cannon_UV_Bump.tif";
	string path = "..\\Clouds\\media\\height_map.png";
//	string path = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\005\\front.jpg";
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
	bool flip;
};