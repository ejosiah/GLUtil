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
#include "player.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

constexpr static int NumCells = 16;
constexpr static float CellWidth = 1.0/NumCells;
constexpr static float HalfCellWidth = CellWidth * 0.5f;

class MazeScene : public Scene {
public:
	MazeScene() :Scene("Maze Generator", Resolution::FHD.width, Resolution::FHD.height) {
		_requireMouse = true;
		useImplictShaderLoad(true);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);
		addShader("mazeMap", GL_VERTEX_SHADER, identity_vert_shader);
		addShader("mazeMap", GL_FRAGMENT_SHADER, identity_frag_shader);
		camInfoOn = false;
		_fullScreen = false;
		_modelHeight = 2.6;
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

		maze.init();
		_3dMaze = make_unique<MazeObject<NumCells, NumCells>>(*this, maze);
		_3dMaze->init();


		mazeMap = make_unique<MazeMap<NumCells, NumCells>>(*this, maze, _3dMaze->getCellWidth());
		mazeMap->init();

		color_tex = new CheckerTexture(1, "diffuse", RED, RED);
		//auto model = translate(mat4(1), { 0, 2, 0 }) * scale(mat4(1), { 6, 4, 1 }) * rotate(mat4(1), half_pi<float>(), { 1, 0, 0 });
		auto model = translate(mat4(1), { 0, 2, 0 });
		model = scale(model, { 6, 4, 1 });
		model = rotate(model, half_pi<float>(), { 1, 0, 0 });
		model = scale(mat4(1), vec3(1000));
		floor = std::unique_ptr< Floor>(new Floor{ 100, *this, model, nullptr, 100.0f });
		floor->init();
		
		lightModel.useObjectSpace = true;
		lightModel.localViewer = true;

		cube = make_unique<Model>("C:\\Users\\" + username + "\\OneDrive\\media\\models\\one_meter_cube.obj");
		sphere = make_unique<Sphere>(0.3, 10, 10);
		sphere->material().diffuse = GREEN;
		initDefaultCamera();
		player.camera = &activeCamera();
		player.camera->setMode(Camera::FIRST_PERSON);
		player.currentCell = &maze.grid[0][0];

		player.updatePosition(vec3(player.currentCell->center.x, 0, player.currentCell->center.y));

		light[0].on = true;
	//	light[0].spotAngle = 30;
		light[0].position = { 0, 1, 0, 0 };
		
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
		shader("floor")([&](Shader& s) {
			glViewportIndexedf(0, 0, 0, _width, _height);
			send("id", 0);
			send("flipNormal", true);
			light[0].position = vec4(player.position(), 1);
			light[0].spotDirection = vec4(player.camera->getViewDirection(), 0);
			send(light[0]);

			lightModel.useObjectSpace = true;
			send(lightModel);
			send(activeCamera());
			shade(_3dMaze.get());

			lightModel.useObjectSpace = false;
			send(lightModel);
			floor->draw(s);
		});


		glDepthFunc(GL_ALWAYS);
		shader("mazeMap")([&](Shader& s) {
			glViewportIndexedf(1, _width - 250, _height - 250, 200, 200);
			cam.projection = ortho(-CellWidth, 1.0f, -CellWidth, 1.0f, -1.0f, 1.0f);
			send(cam);
			send("id", 1);
			send("height_scale", heightScale);
			shade(mazeMap.get());
		});
		

		shader("flat")([&](Shader& s) {
			glViewportIndexedf(1, _width - 250, _height - 250, 200, 200);
			cam.projection = ortho(-CellWidth, 1.0f, -CellWidth, 1.0f, -1.0f, 1.0f);
			send(cam);
			send("id", 1);
			shade(pos.get());
		});
		glDepthFunc(GL_LESS);

		skybox->render();
/*		stringstream ss;
		ss << "positin in maze: " << p;
		sFont->render(ss.str(), 10, 60)*/;
	}

	void update(float t) override {
		auto collision = _3dMaze->collidesWith(player);
		if (collision.happened) {
			player.updatePosition(collision.point);
		}

		vec2 p = mazeMap->to2dPoint(player.position());
		pos->update2<vec3>(VAOObject::Position, [&](vec3* v) {
			v->x = p.x;
			v->y = p.y;
		});
	}

	void processInput(const Key& key) override {
		if (key.pressed() && key.value() == 'y') {
			heightScale += 0.1;
			heightScale = heightScale > 1.0f ? 1.0f : heightScale;
		}
		else if (key.pressed() && key.value() == 'h') {
			heightScale -= 0.1;
			heightScale = heightScale < 0.01 ? 0.01 : heightScale;
		}
	}

private:
	unique_ptr<Floor> floor;
	unique_ptr<ProvidedMesh> pos;
	Maze<NumCells, NumCells> maze;
	unique_ptr<MazeObject<NumCells, NumCells>> _3dMaze;
	unique_ptr<MazeMap<NumCells, NumCells>> mazeMap;
	unique_ptr<ProvidedMesh> background;
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
	Player player;
	float heightScale = 1.0f;
};