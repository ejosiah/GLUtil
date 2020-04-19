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
#include "MazeMap.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

constexpr static int NumCells = 10;
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
		_modelHeight = 1;
	}

	void init() override {
		setBackGroundColor(BLACK);
		setForeGroundColor(WHITE);
		string root = "C:\\Users\\" + username + "\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
		});
		skybox = unique_ptr<SkyBox>{ SkyBox::create(skyTextures, 7, *this, 50) };
		createPoints();

		_3dMaze = make_unique<MazeObject<NumCells, NumCells>>(*this);
		_3dMaze->init();


		mazeMap = make_unique<MazeMap<NumCells, NumCells>>(*this);
		mazeMap->init();

		color_tex = new CheckerTexture(1, "diffuse", RED, RED);
		//auto model = translate(mat4(1), { 0, 2, 0 }) * scale(mat4(1), { 6, 4, 1 }) * rotate(mat4(1), half_pi<float>(), { 1, 0, 0 });
		auto model = translate(mat4(1), { 0, 2, 0 });
		model = scale(model, { 6, 4, 1 });
		model = rotate(model, half_pi<float>(), { 1, 0, 0 });

		wall = make_unique<Floor>(10, *this, model);
		wall->init(color_tex);

		model = scale(mat4(1), vec3(1000));
		floor = make_unique<Floor>(100, *this, model);
		floor->init();

		light[0].on = true;
		light[0].position = { 0, 1, 0, 0 };
		lightModel.useObjectSpace = false;
		lightModel.localViewer = true;

		cube = make_unique<Model>("C:\\Users\\" + username + "\\OneDrive\\media\\models\\one_meter_cube.obj");
		sphere = make_unique<Sphere>(0.3, 10, 10);
		sphere->material().diffuse = GREEN;
	}

	void createPoints() {
		Mesh mesh;
		mesh.positions.push_back(vec3(0));
		mesh.colors.push_back(RED);
		mesh.primitiveType = GL_POINTS;
		pos = make_unique<ProvidedMesh>(mesh);
		glPointSize(5.0);
	}

	void display() override {
		shader("flat")([&](Shader& s) {
			cam.projection = ortho(-CellWidth, 1.0f, -CellWidth, 1.0f, -1.0f, 1.0f);
			send(cam);
			shade(pos.get());
			shade(mazeMap.get());
		});
		
		//shader("floor")([&](Shader& s) {

		//	send(light[0]);
		//	send(lightModel);
		//	send(activeCamera());
		////	wall->draw(s
		//	shade(maze.get());
		//	floor->draw(s);
		//});
		//shader("default")([&](Shader& s) {
		//	send(light[0]);
		//	send(lightModel);
		//	send(activeCamera());
		//	shade(sphere.get());
		//});
		//skybox->render();
		stringstream ss;
		ss << "positin in maze: " << p;
		sFont->render(ss.str(), 10, 60);

	}

	void update(float t) override {
		p = _3dMaze->collidesWith(activeCamera().getPosition());
		pos->update2<vec3>(VAOObject::Position, [&](vec3* v) { 
			v->x = p.x;
			v->y = -p.z;
		});
	}

private:
	unique_ptr<Floor> floor;
	unique_ptr<Floor> wall;
	unique_ptr<ProvidedMesh> pos;
	unique_ptr<MazeObject<NumCells, NumCells>> _3dMaze;
	unique_ptr<MazeMap<NumCells, NumCells>> mazeMap;

	unique_ptr<SkyBox> skybox;
	unique_ptr<Model> cube;
	unique_ptr<Sphere> sphere;
	Texture2D* color_tex;
	vector<string> skyTextures = vector<string>{
		"right.jpg", "left.jpg",
		"top.jpg", "bottom.jpg",
		"front.jpg", "back.jpg"
	};
	vec3 p;
};