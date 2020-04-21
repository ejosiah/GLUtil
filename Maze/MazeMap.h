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
	MazeMap(const Scene& scene, Maze<rows, cols>& maze, float cellWidth) 
		:scene{ scene }
		, maze{ maze }
		, cellWidth{ cellWidth }{
	}

	void init() {
		buildMap(maze);
		buildBackground();
		build3dTo2dMatrix();
	}

	void build3dTo2dMatrix() {
		float dx = cellWidth * 0.5f;
		mat4 _3dxform = translate(mat4{ 1 }, { -dx, 0, -dx });
		_3dxform = scale(_3dxform, { cellWidth * cols, 1, cellWidth * cols });	// TODO move to MazeObj

		dx = (1.0 / cols) * 0.5f;
		_2dXform = translate(mat4{ 1 }, { -dx, 0, -dx });

		_2dXform = _2dXform * inverse(_3dxform);
	}

	void buildBackground() {
		Mesh mesh;
		mesh.positions.push_back(vec3(-1, 1, 0));
		mesh.positions.push_back(vec3(-1,-1, 0));
		mesh.positions.push_back(vec3(1, 1, 0));
		mesh.positions.push_back(vec3(1, -1, 0));
		mesh.colors = vector<vec4>(4, BLACK);
		mesh.material.diffuse = vec4(1, 0, 0, 1);
		mesh.primitiveType = GL_TRIANGLE_STRIP;

		background = make_unique<ProvidedMesh>(vector<Mesh>(1, mesh));
	}

	template<size_t rows, size_t cols>
	void buildMap(Maze<rows, cols>& maze) {
		Mesh mesh;
		const float CellWidth = 1.0 / cols;
		const float CellHeight = 1.0 / rows;
		const float halfWidth = CellWidth * 0.5;
		const float halfHeight = CellHeight * 0.5;

		//vec3 min = vec3{ numeric_limits<float>::max()};
		//vec3 max = vec3{ numeric_limits<float>::min()};

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

			//min = glm::min(min, p0);
			//min = glm::min(min, p1);
			//max = glm::max(max, p0);
			//max = glm::max(max, p1);

		}

		mesh.primitiveType = GL_LINES;
		map = std::make_unique<ProvidedMesh>(mesh);
	}


	vec2 to2dPoint(vec3 p) {
		vec2 res = (_2dXform * vec4(p, 1)).xz;
		return res;
	}

	void draw(Shader& shader) override {
		map->draw(shader);
	//	background->draw(shader);
	}

private:
	RecursiveBackTrackingMazeGenerator generator;
	//	RandomizedKrushkalMazeGenerate generator;
	//	RandomizedPrimsMazeGenerator generator;
	std::unique_ptr<ProvidedMesh> map;
	unique_ptr<ProvidedMesh> background;
	const Scene& scene;
	Maze<rows, cols>& maze;
	float cellWidth;
	mat4 _2dXform;
	Logger& logger = Logger::get("maze");
};