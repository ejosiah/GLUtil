#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Drawable.h"
#include "../GLUtil/include/ncl/gl/ProvidedMesh.h"
#include "../GLUtil/include/ncl/gl/mesh.h"
#include "maze_generator.h"

using namespace ncl;
using namespace gl;
using namespace glm;

static Id path[9]{ {0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}, {1, 1} };

static int i = 0;

template<size_t rows, size_t cols>
class MazeObject : public Drawable {
public:
	MazeObject() {
	}

	void init() {
		createMap();
	}

	void createMap() {
		Mesh mesh;	
		const float CellWidth = 1.0 / cols;
		const float CellHeight = 1.0 / rows;
		const float halfWidth = CellWidth * 0.5;
		const float halfHeight = CellHeight * 0.5;
		std::set<Wall*> processed;
		Maze<rows, cols> maze;
		generator.generate(maze);

		for (int j = 0; j < rows; j++) {
			for (int i = 0; i < cols; i++) {
				vec2 c{ i * CellWidth, j * CellHeight };
				
				Cell& cell = maze.grid[j][i];
				std::list<Wall*> walls = cell.walls;
				
				for (Wall* wall : walls) {
					if (processed.find(wall) != processed.end()) continue;
					vec3 p0, p1;
					switch (cell.locationOf(*wall)) {
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
					processed.insert(wall);
				}
				
			}
		}
		mesh.primitiveType = GL_LINES;
		map = std::make_unique<ProvidedMesh>(mesh);
	}

	void draw(Shader& shader) override {
		map->draw(shader);
	}

private:
	RecursiveBackTrackingMazeGenerator generator;
	std::unique_ptr<ProvidedMesh> map;
	GlmCam cam;
	Logger& logger = Logger::get("maze");
};