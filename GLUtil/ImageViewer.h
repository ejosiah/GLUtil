#pragma once

#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/ncl/gl/ProvidedMesh.h"
#include "include/ncl/gl/Scene.h"
#include "include/ncl/gl/Image.h"
#include "include/ncl/gl/Shader.h"
#include "include/ncl/gl/textures.h"
#include "include/ncl/gl/Noise.h"
#include "include/ncl/gl/compute.h"
#include "include/ncl/gl/pbr.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ImageViewer : public Scene {
public:
	

	ImageViewer() :Scene("Image Viewr", 0, 0, false) {
	//	Image img("C:\\Users\\" + username + "\\OneDrive\\media\\textures\\Portrait-8.jpg");
		Image img("D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\PBR\\textures\\hdr\\newport_loft.hdr");
		//_width = 512;
		//_height = _width * float(img.width())/ img.height();
		_width = img.width();
		_height = img.height();
		_width = 512;
		_height = 512;
		addShaderFromFile("image", "shaders/identity.vert");
		addShaderFromFile("image", "shaders/texture.frag");
	}
	

	virtual void init() override {
	//	init_shader();
		loadTextures();
		createPlane();
		glClearColor(0, 0, 0, 1);
		glEnable(GL_DEPTH_TEST);
	//	glColorMask(1, 0, 0, 1);
	//	glEnable(GL_SCISSOR_TEST);

	//	glScissor(50, 50, GLsizei(_width * 0.75f), GLsizei(_height * 0.75f));

	}


	void loadTextures() {
	//	board = new CheckerBoard_gpu(256, 256, WHITE, GRAY, 1, "image1");
	//	board->compute();
	//	board->images().front().renderMode();
	//	WorleyNoise2D noise(Euclidean, invertLayout);
	//	texture0 = new NoiseTex2D;
	//	texture1 = new NoiseTex3D();
	//	shader("image").use();
	//	texture0 = new CheckerTexture(1, "image");
	//	texture0 = new Texture2D("C:\\Users\\" + username + "\\OneDrive\\media\\textures\\Portrait-8.jpg", 0);
	//	texture0 = load_hdr_texture("D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\PBR\\textures\\hdr\\newport_loft.hdr", 0, "newport_loft");
		
	//	texture0->sendTo(shader("image"));
	//	shader("image").unUse();
		texture1 = new Texture2D("C:\\Users\\" + username + "\\OneDrive\\media\\textures\\old_leather.jpg", 1);
	//	shader("image").sendUniform1ui("image0", texture0->id());
	//	_shader.sendUniform1ui("image1", texture1->id());
	}


	void createPlane() {
		Mesh mesh;
		mesh.positions.push_back(vec3(0, 1, 0));
		mesh.positions.push_back(vec3(0, 0, 0));
		mesh.positions.push_back(vec3(1, 1, 0));
		mesh.positions.push_back(vec3(1, 0, 0));
		mesh.uvs[0].push_back(vec2(0, 1));
		mesh.uvs[0].push_back(vec2(0, 0));
		mesh.uvs[0].push_back(vec2(1, 1));
		mesh.uvs[0].push_back(vec2(1, 0));
		mesh.material.diffuse = vec4(1, 0, 0, 1);
		mesh.primitiveType = GL_TRIANGLE_STRIP;

		plane = new ProvidedMesh(vector<Mesh>(1, mesh));
		cube = new Cube();
	}

	virtual void resized() override{
		projection = ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	}

	virtual void display() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!texture0) {
			texture0 = pbr::generate_brdf_lookup_table(0);
		}

		auto texId0 = texture0->bufferId();
		auto texId1 = texture1->bufferId();

		shader("image")([&](Shader& s) {
			
			//glActiveTexture(GL_TEXTURE0);

		//	glBindTextureUnit(0, texId0);
		//	glBindTexture(GL_TEXTURE_2D, texture0->bufferId());
		//	s.sendUniform1i("image0", texture0->unit());

		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D, texture1->bufferId());
		//	s.sendUniform1i("image1", texture1->unit());
		//	glBindTextureUnit(1, texId1);
			
			send(texture1);
			send(texture0);
		//	send(&board->images().front());
			s.sendUniformMatrix4fv("MVP", 1, GL_FALSE, &projection[0][0]);
			plane->draw(s);
		});
	}

	virtual void update(float elapsedTime) {

	}

private:
	ProvidedMesh* plane;
	mat4 projection;
	Cube* cube;
	Texture2D* texture1;
	Texture2D* texture0 = nullptr;
	CheckerBoard_gpu* board;
	Shader _shader;
};