#pragma once
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../fsim/fields.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include <memory>
#include "../GLUtil/include/glm/vec_util.h"
#include <tuple>
#include "MazeObj.h"
#include "Floor.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

constexpr static int NumCells = 50;
constexpr static float CellWidth = 1.0/NumCells;
constexpr static float HalfCellWidth = CellWidth * 0.5f;

class MazeScene : public Scene {
public:
	MazeScene() :Scene("Maze Generator") {
		_requireMouse = true;
		useImplictShaderLoad(true);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		camInfoOn = true;
		_fullScreen = false;
	}

	void init() override {
		setBackGroundColor(WHITE);
		setForeGroundColor(BLACK);
		string root = "C:\\Users\\" + username + "\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
			});
		skybox = unique_ptr<SkyBox>{ SkyBox::create(skyTextures, 7, *this, 50) };
		createPoints();
		maze.init();

		floor = make_unique<Floor>(100, 1100, *this);
		floor->init();

		light[0].on = true;
		light[0].position = { 0, 1, 0, 0 };
		lightModel.useObjectSpace = false;
		lightModel.localViewer = true;

		cube = make_unique<Model>("C:\\Users\\Josiah\\Documents\\one_meter_cube.obj");
	}

	void createPoints() {
		Mesh mesh;
		for (int j = 0; j < NumCells; j++) {
			for (int i = 0; i < NumCells; i++) {
				float x = CellWidth * i;
				float y =  CellWidth * j;
				mesh.positions.emplace_back(x, y, 0);
				mesh.colors.push_back(getForeGround());
			}
		}
		mesh.primitiveType = GL_POINTS;
		points = make_unique<ProvidedMesh>(mesh);
	}

	void display() override {
		cam.projection = ortho(-CellWidth, 1.0f, -CellWidth, 1.0f, -1.0f, 1.0f);
		shader("flat")([&](Shader& s) {
		//	send(cam);
			//shade(points.get());
		//	shade(&maze);
		});
		
		shader("floor")([&](Shader& s) {

			send(light[0]);
			send(lightModel);
			send(activeCamera());
			floor->draw(s);
		});
		shader("default")([&](Shader& s) {
			send(light[0]);
			send(lightModel);
			send(activeCamera());
			shade(cube.get());
		});
		skybox->render();
	}

private:
	unique_ptr<Floor> floor;
	unique_ptr<ProvidedMesh> points;
	MazeObject<NumCells, NumCells> maze;
	unique_ptr<SkyBox> skybox;
	unique_ptr<Model> cube;
	vector<string> skyTextures = vector<string>{
		"right.jpg", "left.jpg",
		"top.jpg", "bottom.jpg",
		"front.jpg", "back.jpg"
	};
};