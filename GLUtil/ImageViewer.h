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


using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class ImageViewer : public Scene {
public:
	

	ImageViewer() :Scene("Image Viewr", 0, 0, false) {
		Image img("C:\\Users\\" + username + "\\OneDrive\\media\\textures\\Portrait-8.jpg");
		_width = 512;
		_height = _width * float(img.width())/ img.height();
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

		WorleyNoise2D noise(Euclidean, invertLayout);
	//	texture0 = new NoiseTex2D;
	//	texture1 = new NoiseTex3D();
		shader("image").use();
	//	texture0 = new Texture2D("C:\\Users\\" + username + "\\OneDrive\\media\\textures\\Portrait-8.jpg");
		texture0->sendTo(shader("image"));
		texture0 = new CheckerTexture(0, "image");
	//	texture1 = new Texture2D("D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\Butterfiles\\media\\butterfly-for-imaginal-cells.png", 1);
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

		shader("image")([&](Shader& s) {
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
	Texture3D* texture1;
	Texture2D* texture0;
	Shader _shader;
};