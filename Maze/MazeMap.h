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
class MazeMap : public Drawable {
public:
	MazeMap(const Scene& scene) :scene{ scene } {
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

		buildMap(maze);
	}

	template<size_t rows, size_t cols>
	void buildMap(Maze<rows, cols>& maze) {
		Mesh mesh;
		const float CellWidth = 1.0 / cols;
		const float CellHeight = 1.0 / rows;
		const float halfWidth = CellWidth * 0.5;
		const float halfHeight = CellHeight * 0.5;

		auto walls = maze.walls();
		for (auto wall : walls) {
			Cell& cell = *wall->left;
			vec2 c{ cell.id.col * CellWidth, cell.id.row * CellHeight };
			vec3 p0, p1;
			switch (wall->location) {
			case Location::Top:
				p0 = { c.x - halfWidth, c.y + halfHeight, 0 };
				p1 = { c.x + halfWidth, c.y + halfHeight, 0 };
				break;
			case Location::Right:
				p0 = { c.x + halfWidth, c.y + halfWidth, 0 };
				p1 = { c.x + halfWidth, c.y - halfHeight, 0 };
				break;
			case Location::Bottom:
				p0 = { c.x - halfHeight, c.y - halfHeight, 0 };
				p1 = { c.x + halfHeight, c.y - halfHeight, 0 };
				break;
			case Location::Left:
				p0 = { c.x - halfWidth, c.y + halfHeight, 0 };
				p1 = { c.x - halfWidth, c.y - halfHeight, 0 };
				break;
			}
			mesh.positions.push_back(p0);
			mesh.positions.push_back(p1);
			mesh.colors.push_back(WHITE);
			mesh.colors.push_back(WHITE);

		}

		mesh.primitiveType = GL_LINES;
		map = std::make_unique<ProvidedMesh>(mesh);
	}


	void draw(Shader& shader) override {
		map->draw(shader);
	}

private:
	RecursiveBackTrackingMazeGenerator generator;
	//	RandomizedKrushkalMazeGenerate generator;
	//	RandomizedPrimsMazeGenerator generator;
	std::unique_ptr<ProvidedMesh> map;
	const Scene& scene;
	Logger& logger = Logger::get("maze");
};