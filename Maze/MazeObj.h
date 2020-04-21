#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Drawable.h"
#include "../GLUtil/include/ncl/gl/ProvidedMesh.h"
#include "../GLUtil/include/ncl/gl/mesh.h"
#include "../GLUtil/include/ncl/util/profile.h"
#include "../GLUtil/include/ncl/geom/Plane.h"
#include "maze_generator.h"
#include "Floor.h"
#include <chrono>
#include "player.h"
#include "../GLUtil/include/ncl/data_structure/quad_tree.h"


using namespace ncl;
using namespace gl;
using namespace glm;
using namespace std;

struct Collision {
	bool happened;
	vec3 point;
};


template<size_t rows, size_t cols>
class MazeObject : public Drawable {
public:
	MazeObject(const Scene& scene, Maze<rows, cols>& maze, float cellWidth = 4, float cellHeight = 3)
		:scene{ scene }
		, maze{ maze }
		, cellWidth{ cellWidth }
		, cellHeight{ cellHeight }{

		
	}

	void init() {
		auto duration = profile([&]() { generator.generate(maze);  });
		logger.info("maze generated in " + print(duration));
		setMazeExits();
		buildMazeCoordinates();
		buildQuadTree();
		build3dMaze(maze);
	}

	void setMazeExits() {
		auto bottomLeft = maze.cellAt({ 0, 0 });
		delete bottomLeft->wallAt(Location::Bottom);

		auto topRight = maze.cellAt({ rows - 1, cols - 1 });
		delete topRight->wallAt(Location::Right);
	}

	void buildMazeCoordinates() {
		maze.foreach([&](Cell* cell) {
			vec2 c = vec2{ cell->id.col * cellWidth, cell->id.row * cellWidth };
			auto dl = cellWidth / 2;
			cell->bounds.min = vec2{ c.x - dl, c.y - dl };
			cell->bounds.max = vec2{ c.x + dl, c.y + dl };
			cell->center = c;

		});
	}

	void buildQuadTree() {
		auto min = maze.cellAt({ 0, 0 })->bounds.min;
		auto max = maze.cellAt({ rows - 1, cols - 1 })->bounds.max;

		quadTree = ds::quad_tree<Cell>{ min, max, cellWidth };
		maze.foreach([&](Cell* cell) {
			quadTree.insert(new ds::Node<Cell>{ cell->center, cell });
		});
	}

	template<size_t rows, size_t cols>
	void build3dMaze(Maze<rows, cols>& maze) {

		Mesh mesh;
		const float halfWidth = cellWidth * 0.5;
		const float halfHeight = cellHeight * 0.5;

		std::vector<mat4> models;

		//auto walls = maze.walls();
		//for (auto wall : walls) {
		//	Cell& cell = *wall->left;
		//	if (wall->location == Location::Top || wall->location == Location::Bottom) continue;
		//	vec3 c{ cell.id.col * 0.5, 0, cell.id.row * 0.5 };

		//	glm::mat4 model{ 1 };

		//	//model = move(wall->location, model, halfWidth);
		//	//model = translate(model, { 0, halfHeight, 0 });
		////	model = scale(model, { cellWidth, cellHeight, 1 });
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
					if (processed.find(wall) != processed.end()) continue;
					auto location = cell.locationOf(*wall);
					
					glm::mat4 model{ 1 };

					//model = move(wall->location, model, halfWidth);
					//model = translate(model, { 0, halfHeight, 0 });

					model = translate(model, { cell.id.col * cellWidth, 0, cell.id.row * cellWidth });
					model = translate(model, { 0, halfHeight, 0 });
					model = move(location, model, halfWidth, cell.id);
					model = orient(location, model);
					model = rotate(model, glm::half_pi<float>(), { 1, 0, 0 });
					model = scale(model, { cellWidth, 1, cellWidth });

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

	glm::mat4 orient(Location location, glm::mat4 xform) {
		glm::mat4 res = xform;
		if (location == Location::Left) {
			res = rotate(res, half_pi<float>(), { 0, 1, 0 });
		}
		else if (location == Location::Right) {
			res = rotate(res, -half_pi<float>(), { 0, 1, 0 });
		}
		else if (location == Location::Top) {
			res = rotate(res, pi<float>(), { 1, 0, 0 });
		}
		return res;
	}

	glm::mat4 move(Location location, glm::mat4 xform, float w, Id id) {
		glm::mat4 model;
		float x = 0;
		switch (location) {
		case Location::Top:
			model = translate(xform, { 0, 0, w });
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

	Collision collidesWith(vec3 pos) {
		ds::Node<Cell>* node = quadTree.search(pos.xz);

		if (node) {
			Cell& cell = *node->data;

			if (cell.contains(pos)) {
				auto walls = cell.walls;
				for (auto wall : walls) {
					auto collision = test(*wall, cell, pos);
					if (collision.happened) return collision;
				}
			}
		}
		else {

			ss.str("");
			ss.clear();
			ss << "position: " << pos << ", outside maze";
			logger.info(ss.str());
		}

		return { false, vec3(0) };
	}

	Collision test(Wall& wall, Cell& cell, vec3 pos) {
		auto plane = planeOf(wall, cell);
		float t = dot(plane.n, pos);
		if (t < (plane.d + 0.2f)) {
			auto p = projectToPlane(pos, plane, 0.2f);
			return { true, p };
		}
		else {
			return { false, vec3(0) };
		}
	}

	Collision collidesWith(const Player& player) {
		return collidesWith(player.position());
	}

	vector<geom::Plane> wallsOf(Cell& cell) {
		vector<geom::Plane> rtVal;
		for (auto wall : cell.walls) {
			auto xform = xformOf(cell, *wall);
			vec3 p = (xform * vec4(0, 0, 0, 1)).xyz;
			vec3 n = normalOf(cell.locationOf(*wall));
			rtVal.push_back(geom::Plane{ n, dot(n, p) });
		}
		return rtVal;
	}

	geom::Plane planeOf(Wall& wall, Cell& cell) {
		auto xform = xformOf(cell, wall);
		vec3 p = (xform * vec4(0, 0, 0, 1)).xyz;
		vec3 n = normalOf(cell.locationOf(wall));
		return { n, dot(n, p) };
	}

	mat4 xformOf(Cell& cell, Wall& wall) {
		// TODO get the data from wall xform attribute
		const float halfWidth = cellWidth * 0.5;
		const float halfHeight = cellHeight * 0.5;

		glm::mat4 model{ 1 };

		//model = move(wall->location, model, halfWidth);
		//model = translate(model, { 0, halfHeight, 0 });
		auto location = cell.locationOf(wall);
		model = translate(model, { cell.id.col * cellWidth, 0, cell.id.row * cellWidth });
		model = translate(model, { 0, halfHeight, 0 });
		model = move(location, model, halfWidth, cell.id);
		model = orient(location, model);
		model = rotate(model, glm::half_pi<float>(), { 1, 0, 0 });
		model = scale(model, { cellWidth, 1, cellWidth });

		return model;
	}

	vec3 normalOf(Location location) {
		switch (location) {
		case Location::Top:
			return { 0, 0, -1 };
		case Location::Right:
			return { -1, 0, 0 };
		case Location::Bottom:
			return { 0, 0, 1 };
		case Location::Left:
			return { 1, 0, 0 };
		}
	}

	float getCellWidth() {
		return cellWidth;
	}

private:
	RecursiveBackTrackingMazeGenerator generator;
//	RandomizedKrushkalMazeGenerate generator;
//	RandomizedPrimsMazeGenerator generator;
	Texture2D* color_tex;
	unique_ptr<Floor> wall;
	GlmCam cam;
	const Scene& scene;
	Maze<rows, cols>& maze;
	float cellWidth, cellHeight;
	Logger& logger = Logger::get("maze");
	ds::quad_tree<Cell> quadTree;
	stringstream ss;
};