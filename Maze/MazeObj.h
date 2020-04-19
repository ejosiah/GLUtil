#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Drawable.h"
#include "../GLUtil/include/ncl/gl/ProvidedMesh.h"
#include "../GLUtil/include/ncl/gl/mesh.h"
#include "../GLUtil/include/ncl/util/profile.h"
#include "maze_generator.h"
#include "Floor.h"
#include <chrono>


using namespace ncl;
using namespace gl;
using namespace glm;
using namespace std;


template<size_t rows, size_t cols>
class MazeObject : public Drawable {
public:
	MazeObject(const Scene& scene):scene{ scene }{
	}

	void init() {
		createMap();
	}

	void createMap() {
		Maze<rows, cols> maze;
		maze.init();
		auto duration = profile([&]() { generator.generate(maze);  });

//		logger.info("maze generated in " + print(duration));

		auto bottomLeft = maze.cellAt({ 0, 0 });
		delete bottomLeft->wallAt(Location::Bottom);

		auto topRight = maze.cellAt({ rows - 1, cols - 1 });
		delete topRight->wallAt(Location::Right);

		build3dMaze(maze);
	}

	template<size_t rows, size_t cols>
	void build3dMaze(Maze<rows, cols>& maze) {

		Mesh mesh;
		const float CellWidth = 4;
		const float CellHeight = 3;
		const float halfWidth = CellWidth * 0.5;
		const float halfHeight = CellHeight * 0.5;

		std::vector<mat4> models;

		//auto walls = maze.walls();
		//for (auto wall : walls) {
		//	Cell& cell = *wall->left;
		//	if (wall->location == Location::Top || wall->location == Location::Bottom) continue;
		//	vec3 c{ cell.id.col * 0.5, 0, cell.id.row * 0.5 };

		//	glm::mat4 model{ 1 };

		//	//model = move(wall->location, model, halfWidth);
		//	//model = translate(model, { 0, halfHeight, 0 });
		////	model = scale(model, { CellWidth, CellHeight, 1 });
		//	
		//	model = translate(model, { 0, 0.5, 0 });
		//	model = move(wall->location, model, 0.5);
		//	model = translate(model, c);
		//	model = orient(wall->location, model, c);
		//	model = rotate(model, glm::half_pi<float>(), { 1, 0, 0 });
		//	models.push_back(model);
		//	stringstream ss;
		//	ss << "id: [" << cell.id.col << ", "  << cell.id.row << "], " << "c: [" << c.x << ", " << c.y << ", " << c.z << "]";
		//	logger.info(ss.str());
		//}

		std::set<Wall*> processed;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {

				Cell& cell = maze.grid[i][j];
			//	Cell& cell = maze.grid[0][0];
			//	vec3 c{ cell.id.col * 0.5, 0, cell.id.row * 0.5 };

				std::list<Wall*> walls = cell.walls;

				for (Wall* wall : walls) {
					auto flag = processed.find(wall) != processed.end();
					if (processed.find(wall) != processed.end()) continue;
					auto location = cell.locationOf(*wall);
					
					vec3 c{ 0 };

					glm::mat4 model{ 1 };

					//model = move(wall->location, model, halfWidth);
					//model = translate(model, { 0, halfHeight, 0 });

					model = translate(model, { cell.id.col * CellWidth, 0, cell.id.row * CellWidth });
					model = translate(model, { 0, halfHeight, 0 });
					model = move(location, model, halfWidth, cell.id);
					model = translate(model, c);
					model = orient(location, model, c);
					model = rotate(model, glm::half_pi<float>(), { 1, 0, 0 });
					model = scale(model, { CellWidth, 1, CellWidth });

					vec4 p = model * vec4(0, 0, 0, 1);

					//stringstream ss;
					//if (wall->location == Location::Top || wall->location == Location::Bottom) {
					//	ss << "cell[" << cell.id.col << ", " << cell.id.row << "], wall: " << toString(wall->location);
					//	ss << ", p[" << p.x << ", " << p.y << ", " << p.z << "]";
					//}

					//logger.info(ss.str());
					models.push_back(model);
					processed.insert(wall);
				}

			}
		}

		color_tex = new CheckerTexture(1, "diffuse", RED, RED);
		wall = make_unique<Floor>(4, scene, models);
		wall->init(color_tex);
	}

	glm::mat4 orient(Location location, glm::mat4 xform, vec3 c) {
		glm::mat4 res = xform;
		if (location == Location::Left) {
	//		res = translate(xform, c);
			res = rotate(res, half_pi<float>(), { 0, 1, 0 });
		//	res = translate(res, -c);
		}
		else if (location == Location::Right) {
	//		res = translate(res, c);
			res = rotate(res, -half_pi<float>(), { 0, 1, 0 });
	//		res = translate(res, -c);
		}
		return res;
	}

	glm::mat4 move(Location location, glm::mat4 xform, float w, Id id) {
		glm::mat4 model;
		float x = 0;
		switch (location) {
		case Location::Top:
			return translate(xform, { 0, 0, w });
			break;
		case Location::Right:
			model = translate(xform, {w, 0, 0});
			break;
		case Location::Bottom:
			model = translate(xform, { 0, 0, -w });
			break;
		case Location::Left:
			model = translate(xform, { -w, 0, 0 });
			break;
		}
		return model;
	}

	void draw(Shader& shader) override {
		wall->draw(shader);
	}

	vec3 collidesWith(vec3 pos) {
		mat4 mat = inverse(scale(mat4(1), { 4, 1, 4 }));
		vec3 p = vec3(mat * vec4(pos, 1));
		return p;
	}

private:
	RecursiveBackTrackingMazeGenerator generator;
//	RandomizedKrushkalMazeGenerate generator;
//	RandomizedPrimsMazeGenerator generator;
	Texture2D* color_tex;
	unique_ptr<Floor> wall;
	GlmCam cam;
	const Scene& scene;
	Logger& logger = Logger::get("maze");
};