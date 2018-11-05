#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/ncl/gl/primitives.h"
#include "include/ncl/gl/models.h"
#include "include/ncl/geom/Plane.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class _3DMotionEventLogger : public _3DMotionEventHandler {
public:
	_3DMotionEventLogger(GlmCam& cam) :cam{ cam } {
	}

	Logger logger = Logger::get("3D Motion Event");

	virtual void onMotion(const _3DMotionEvent& event) override {
		//string msg;
		//msg = "translation[" + to_string(event.translation.x) + ", " + to_string(event.translation.y) + ", " + to_string(event.translation.z) +  "]";
		//logger.info(msg);
		//msg = "rotation[" + to_string(event.rotation.x) + ", " + to_string(event.rotation.y) + ", " + to_string(event.rotation.z) + "]";
		//logger.info(msg);
		auto trans = event.translation * vec3 {1, -1, 1};
		
		cam.view = rotate(cam.view, radians(event.rotation.z * 0.001f), vec3(0, 0, 1));
		cam.view = rotate(cam.view, radians(event.rotation.y * 0.001f), vec3(0, 1, 0));
		cam.view = rotate(cam.view, radians(event.rotation.x * 0.001f), vec3(1, 0, 0));
		cam.view = translate(cam.view, event.translation * (vec3(1, 1, -1) * 0.00011f));
 	};
	virtual void onNoMotion() override {};

private:
	GlmCam& cam;
};


class ExampleScene : public Scene {


public:
	ExampleScene(Options ops) :Scene("Example Scene", ops) {
		//addShaderFromFile("shaders\\quad.vert");
		//addShaderFromFile("shaders\\quad.tes");
		//addShaderFromFile("shaders\\identity.frag");
		_requireMouse = false;
	}

	virtual void init() override {
		delete _motionEventHandler;
		_motionEventHandler = new _3DMotionEventLogger(cam);
		cam.view = glm::lookAt(vec3(0.0, 0.0, 1.25f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		font = Font::Arial(20, 0, BLACK);
		sphere = new Sphere(0.1, 20, 20);
		teapot = new Teapot(8);
		cube = new Cube;
		cylinder = new Cylinder;
		cone = new Cone;

		geom::Plane p{ { 0, 1, 0 }, 0 };

		plane = new Plane(p, 5.0f, BLACK);
	//	plane = new Plane(10, 10, 1, 1, BLACK);
	//	v = new Vector(vec3{4, 5, 0});
		x = new Vector(vec3{ 1, 0, 0 }, vec3(0), 0.5, RED);
		y = new Vector(vec3{ 0, 1, 0 }, vec3(0), 0.5, GREEN);
		z = new Vector(vec3{ 0, 0, -1 }, vec3(0), 0.5, BLUE);
		model = new Model("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\models\\bigship1.obj", true);
		lightModel.twoSided = false;
		lightModel.colorMaterial = true;
		m.ambient = m.diffuse = vec4(1, 0, 0, -1);
		m.specular = vec4(1);
		shader("default").sendUniformMaterial("material[1]", m);
		glClearColor(0.8, 0.8, 0.8, 1);
		cam.model = mat4(1);
		copyCube();
		aabb = new AABBShape(*sphere);
	}

	virtual void resized() override {
		cam.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
	}

	virtual void display() override {
		cam.model = mat4(1);
	//	_shader.sendUniform1i("grids", 16);
	//	cam.model = mat4(1);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
	//	_shader.send(cam);
		shader("default")([&](Shader& s) {
			s.send(lightModel);
			s.sendUniformLight("light[0]", light[0]);
			s.sendComputed(cam);
		//	shade(plane);
		//	cylinder->draw(s);
			//sphere->draw(s);
			
			//cam.model = translate(mat4(1), { 0, 0, -2 });
			//s.sendComputed(cam);
			//cone->draw(s);
			x->draw(s);
			y->draw(s);
			z->draw(s);
		//	cube->draw(s);
		//	teapot->draw(s);
		});

		shader("flat")([&]() {
	//		send(cam);
	//		shade(aabb);
	//		glFrontFace(GL_CW);
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		shade(plane);
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});

//		cam.model = translate(mat4(1), { 0, 1, 0 });


	//	_shader.send(cam);
	//	sphere->draw(_shader);
	//	UI::render();
	//	font->render("The Quick Brown Fox Jumps over the Lazy Dog", 0, _height - 20);
	}

	void copyCube() {
		vector<GLuint*> buffers = cube->getBuffers();
		GLuint* buf0 = buffers[0];
		for (int i = 0; i < 3; i++) {
			Logger::get("example").info(std::to_string(buf0[i]));
		}
	
		GLint size;
		glBindBuffer(GL_COPY_READ_BUFFER, buf0[0]);
		glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

		Logger::get("example").info("size of pos: " + to_string(size / (sizeof(float) * 3)));

		glGenBuffers(1, &cubeCopyVBOID);
		glBindBuffer(GL_COPY_WRITE_BUFFER, cubeCopyVBOID);
		glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

		glBindBuffer(GL_ARRAY_BUFFER, cubeCopyVBOID);
		glm::vec3* data = (glm::vec3*)glMapNamedBuffer(cubeCopyVBOID, GL_READ_ONLY);
		for (int i = 0; i < 24; i++) {
			glm::vec3 v = *data;
			Logger::get("example").info("v{" + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + "}");
			data++;
		}

	}

	virtual void update(float elapsedTime) override {

	}

private:
	Cube* cube;
	Model* model;
	Sphere* sphere;
	Teapot* teapot;
	Plane* plane;
	Cylinder* cylinder;
	Cone* cone;
	Vector* x;
	Vector* y;
	Vector* z;
	ProvidedMesh* controlPoints;
	ProvidedMesh* controlLines;
	Material m;
	AABBShape* aabb;
	Font* font;
	GLuint cubeCopyVAOID;
	GLuint cubeCopyVBOID;
	
};