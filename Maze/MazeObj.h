#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Drawable.h"
#include "../GLUtil/include/ncl/gl/mesh.h"
#include "maze_generator.h"

using namespace ncl::gl;
using namespace glm;

template<size_t rows, size_t cols>
class MazeObject : public Maze<rows, cols>, Drawable {
public:
	MazeObject() :Maze() {}

	void createMap() {
		Mesh mesh;	
		const float CellWidth = 1.0 / cols;
		const float CellHeight = 1.0 / rows;
		const float halfWidth = CellWidth * 0.5;
		const float halfHeight = CellHeight * 0.5;
		std::set<Wall*> processed;
		for (int j = 0; j < rows; j++) {
			for (int i = 0; i < cols; i++) {
				vec2 c{ i * CellWidth, j * CellHeight };
				
				// top
				mesh.positions.emplace_back{c.x - halfWidth, c.y + halfWidth}
				
			}
		}
	}

	void draw(Shader& shader) override {
		map.draw(shader);
	}

private:

	std::unique_ptr<ProvideMesh> map;
	GlmCam cam;
};