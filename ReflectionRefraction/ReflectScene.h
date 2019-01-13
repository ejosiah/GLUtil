#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/ui/ui.h"
#include "../GLUtil/include/ncl/units/units.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../GLUtil/include/ncl/geom/bvh2.h"


using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace unit;

const string USERNAME = getEnv("username");

class ReflectRefractScene : public Scene {
public:
	ReflectRefractScene() :Scene("Reflect Refract Scene", Resolution::FHD) {
		_useImplictShaderLoad = true;
		_fullScreen = false;
		camInfoOn = true;
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
	}

	void init() override {
		string root = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(faces.begin(), faces.end(), faces.begin(), [&root](string path) {
			return root + path;
		});
		
		skybox = SkyBox::create(faces, 7, *this, 1);
		activeCamera().lookAt({ 0, 0, 5 }, vec3(0), { 0, 1, 0 });
		lucy = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\Lucy-statue\\metallic-lucy-statue-stanford-scan.obj", true, 10);

		GLuint size = lucy->numTriangles() * 3 * sizeof(vec3);
		triangles = new TextureBuffer("triangles", nullptr, size);
		lucy->enableTransformFeedBack(triangles->buffer());
		glEnable(GL_RASTERIZER_DISCARD);
		shader("reflect")([&] {
			send("reflectMode", reflectMode);
			send("doubleRefract", doubleRefract);
			send(activeCamera());
			shade(lucy);
		});
		glDisable(GL_RASTERIZER_DISCARD);
		lucy->disableTransformFeedBack();
		//buildBVH();
		
	}

	void display() override {

		
		shader("reflect")([&] {
			send("reflectMode", reflectMode);
			send("doubleRefract", doubleRefract);
			send("numTriangles", lucy->numTriangles());
			send(activeCamera());
			send(triangles);
			shade(lucy);
		});

	
		skybox->render();


		float x = _width - 200;
		sbr << (reflectMode ? "reflecting" : "refracting") << endl;
		if(!reflectMode) sbr << "double refraction: " << (doubleRefract ? "enabled" : "disabled");
		sFont->render(sbr.str(), x, 10);
		sbr.str("");
		sbr.clear();
	}

	void update(float dt) override {

	}

	void resized() override {

	}

	void processInput(const Key& key) override {
		if (key.released()) {
			if (key.value() == 'm') reflectMode = true;
			if (key.value() == 't') reflectMode = false;
			if (key.value() == 'x') doubleRefract = !doubleRefract;
		}
	}

	//void buildBVH() {
	//	auto bufferId = triangles->buffer();
	//	GLint size;
	//	glBindBuffer(GL_ARRAY_BUFFER, bufferId); CHECK_GL_ERRORS
	//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); 
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	size /= sizeof(vec4) * 3;

	//	vec4* v = (vec4*)glMapNamedBuffer(bufferId, GL_READ_ONLY);

	//	BVH = geom::bvh::create(v, size, 10, size / 30);
	//	geom::bvh::buildLinearBVH(*BVH, bvh_ssbo, bvh_index);;

	//	shader("reflect")([&](Shader& s) {
	//		geom::bvh::send_ssbo(bvh_id, 0, sizeof(geom::bvh::LBVHNode) * bvh_ssbo.nodes.size(), &bvh_ssbo.nodes[0]);
	//		geom::bvh::send_ssbo(bvh_index_id, 1, sizeof(int) * bvh_index.data.size(), &bvh_index.data[0]);
	//	});
	//	glUnmapNamedBuffer(bufferId);
	//}

private:
	SkyBox * skybox;
	Model* lucy;
	bool reflectMode = true;
	bool doubleRefract = false;
	TextureBuffer* triangles;
	vector<string> faces = vector<string>{
		"right.jpg", "left.jpg",
		"top.jpg", "bottom.jpg",
		"front.jpg", "back.jpg"
	};
	GLuint vao;
	GLuint vbo;
	//geom::bvh::BVHNode* BVH;
	//vector<AABBShape*> bvh_widget;
	//geom::bvh::BVH_SSO bvh_ssbo;
	//geom::bvh::BVH_TRI_INDEX bvh_index;
	//GLuint bvh_id;
	//GLuint bvh_index_id;
};