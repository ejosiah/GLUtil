#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class SphericalHarmonicsScene : public Scene {
public:
	SphericalHarmonicsScene() :Scene("Spherical Harmonics") {
		_modelHeight = radius * 2;
		addShader("equi_rect", GL_VERTEX_SHADER, equi_rectangular_map_vert_shader);
		addShader("equi_rect", GL_FRAGMENT_SHADER, equi_rectangular_map_frag_shader);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);
		addShader("irradiance_convolution", GL_VERTEX_SHADER, irradiance_convolution_vert_shader);
		addShader("irradiance_convolution", GL_FRAGMENT_SHADER, irradiance_convolution_frag_shader);
	}

	void init() override {
		initDefaultCamera();
		activeCamera().setMode(Camera::ORBIT);
		cam.model = mat4{ 1 };
		cam.view = lookAt(camPos, vec3(0), { 0, 1, 0 });
		
		sphere = Sphere{ radius, 50, 50, RED };
		equi_rect = load_hdr_texture("..\\PBR\\textures\\hdr\\newport_loft.hdr", 0, "equirectangularMap");
		//equi_rect = load_hdr_texture("C:\\Users\\Josiah\\OneDrive\\media\\textures\\grace_probe.hdr", 0, "equirectangularMap");
		initSkyBox();

		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupSize[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);


		initCoefficientBuffer();
		initGraceCathedralSHBuffer();
		initTotalsBuffer();
		shader("sh")([&] {
			glBindTextureUnit(0, skybox->buffer);
			glBindImageTexture(0, coeff_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			glBindImageTexture(1, totals_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
			auto error = glGetError();
			assert(error == GL_NO_ERROR);
			glDispatchCompute(9, 1, 1);
		});

		vec4* itr = (vec4*)glMapNamedBuffer(coeff_buffer, GL_READ_ONLY);
		for (auto end = itr + 9; itr != end; itr++) {
			vec3 coeff = vec3(*itr);
			ss << coeff << "\n";
		}
		glUnmapNamedBuffer(coeff_buffer);
		ss << "max workgroup invocations: " << &maxWorkGroupSize[0];
		logger.info(ss.str());
	}

	void initSkyBox() {
	//	defer([&]() {

			skybox = SkyBox::create(shader("equi_rect"), 7, *this, *equi_rect);

			auto texture = new Texture2D{ skybox->buffer, 0 };
			irradiance = SkyBox::create(shader("irradiance_convolution"), 5, *this, *texture, 32, 32);

	//	});
	}

	void initCoefficientBuffer() {
		GLuint size = sizeof(vec4) * 9;
		glGenBuffers(1, &coeff_buffer);
		glBindBuffer(GL_TEXTURE_BUFFER, coeff_buffer);
		glBufferData(GL_TEXTURE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW | GL_DYNAMIC_READ);

		glGenTextures(1, &coeff_texture);
		glBindTexture(GL_TEXTURE_BUFFER, coeff_texture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, coeff_buffer);

		assert(glGetError() == GL_NO_ERROR);
	}

	void initTotalsBuffer() {
		GLuint size = sizeof(vec4) * 90;
		glGenBuffers(1, &totals_buffer);
		glBindBuffer(GL_TEXTURE_BUFFER, totals_buffer);
		glBufferData(GL_TEXTURE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW | GL_DYNAMIC_READ);

		glGenTextures(1, &totals_texture);
		glBindTexture(GL_TEXTURE_BUFFER, totals_texture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, totals_buffer);
	}

	void initGraceCathedralSHBuffer() {
		auto grace = graceCathedral();
		GLuint size = sizeof(vec4) * grace.size();
		glGenBuffers(1, &grace_buffer);
		glBindBuffer(GL_TEXTURE_BUFFER, grace_buffer);
		glBufferData(GL_TEXTURE_BUFFER, size, &grace[0], GL_DYNAMIC_DRAW | GL_DYNAMIC_READ);

		glGenTextures(1, &grace_texture);
		glBindTexture(GL_TEXTURE_BUFFER, grace_texture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, grace_buffer);

		assert(glGetError() == GL_NO_ERROR);
	}

	void display() override {



	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		cam.view = lookAt(camPos, vec3(0), { 0, 1, 0 });
		sphere.defautMaterial(false);

		shader("convolution_preview")([&] {
			cam.projection = perspective(pi<float>() / 3, halfWidth /_height, 0.1f, 100.0f);
			glViewportIndexedf(0, 0, 0, halfWidth, _height);
			glBindTextureUnit(0, irradiance->buffer);
			glBindTextureUnit(1, equi_rect->bufferId());
			send("viewId", 0);
			send(cam);
			shade(sphere);
		});

		//shader("sh_lighting")([&] {
		//	cam.projection = perspective(pi<float>() / 3, halfWidth /_height, 0.1f, 100.0f);
		//	glViewportIndexedf(0, 0, 0, halfWidth, _height);
		//	glBindTextureUnit(0, coeff_texture);
		//	//glBindTextureUnit(0, grace_texture);
		//	send("viewId", 0);
		//	send(cam);
		//	shade(sphere);
		//});
		shader("sh_lighting")([&] {
			cam.projection = perspective(pi<float>() / 3, halfWidth / _height, 0.1f, 100.0f);
			glViewportIndexedf(1, halfWidth, 0, halfWidth, _height);
			glBindTextureUnit(0, coeff_texture);
			//glBindTextureUnit(0, grace_texture);
			send("viewId", 1);
			send(cam);
			shade(sphere);
		});
	//	skybox->render();
	//	sFont->render(ss.str(), 10, 10);
	}

	void resized() override {
		halfWidth = _width * 0.5;
		cam.projection = perspective(pi<float>() / 3, aspectRatio, 0.1f, 100.0f);
	}

	void update(float t) {
		elapstedTime += t;
		float _2PI = two_pi<float>();
		camPos.x = 3 * cos(0.05 * _2PI * elapstedTime);
		camPos.z = 3 * sin(0.05 * _2PI * elapstedTime);
	}

	vector<vec4> graceCathedral() {
		return 
		{
			vec4(0.79, 0.44, 0.54, 1),
			vec4(0.39, 0.35, 0.60, 1),
			vec4(-0.34, -0.18, -0.27, 1),
			vec4(-0.29, -0.06, 0.01, 1),
			vec4(-0.11, -0.05, -0.12, 1),
			vec4(-0.26, -0.22, -0.47, 1),
			vec4(-0.16, -0.09, -0.15, 1),
			vec4(0.56, 0.21, 0.14, 1),
			vec4(0.21, -0.05, -.30, 1)
		};
	}

private:
	Texture2D* equi_rect;
	Logger logger = Logger::get("SH");
	Sphere sphere;
	float radius = 1;
	float elapstedTime = 0;
	float halfWidth;
	vec3 camPos = { 0, 0, 3 };
	SkyBox* skybox = nullptr;
	SkyBox* irradiance;
	GLint maxWorkGroupSize[3];
	stringstream ss;
	GLuint coeff_buffer;
	GLuint coeff_texture;
	GLuint totals_buffer;
	GLuint totals_texture;
	GLuint grace_buffer;
	GLuint grace_texture;
	GLuint imageUnit = 0;
};