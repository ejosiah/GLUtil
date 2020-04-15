#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "MazeScene.h"
#include <iostream>

using namespace std;

static Cell* noCell = nullptr;

void checkWallBetween() {
	Cell a{0, 0};
	Cell b{0, 1};
	Cell c{1, 1};

	Wall ac{ &a, &c, Location::Top };

	Wall bc{ &b, &c, Location::Left };

	assert(a.wallBetween(&c) == &ac);
	assert(a.wallBetween(&c) == &ac);

	assert(b.wallBetween(&c) == &bc);
	assert(c.wallBetween(&b) == &bc);

	assert(b.wallBetween(&a) == nullptr);
	assert(a.wallBetween(&b) == nullptr);

	assert(a.wallBetween(&a) == nullptr);
}

void checkHasWallBetweenNeighbours() {
	Cell a{0, 0};
	Cell b{0, 1};
	Cell c{1, 1};

	Wall ac{ &a, &c, Location::Top };
	Wall bc{ &b, &c, Location::Left };


	assert(a.hasWallBetween(&c));
	assert(c.hasWallBetween(&a));

	assert(b.hasWallBetween(&c));
	assert(c.hasWallBetween(&b));

	assert(!b.hasWallBetween(&a));
	assert(!a.hasWallBetween(&b));

	assert(!a.hasWallBetween(&a));
}

void checkRemoveWall() {
	Cell a{0, 0};
	Cell c{1, 1};

	Wall*  ac = new Wall{ &a, &c, Location::Top };


	assert(a.hasWallBetween(&c));
	assert(c.hasWallBetween(&a));

	delete ac;

	assert(!a.hasWallBetween(&c));
	assert(!c.hasWallBetween(&a));

}


void checkWallAt() {
	Cell a{0, 0};
	Cell b{0, 1};
	Cell c{1, 1};

	Wall ac{ &a, &c, Location::Top };
	Wall bc{ &c, &b, Location::Top };

	assert(a.wallAt(Location::Top) == &ac);
	assert(c.wallAt(Location::Bottom) == &ac);

	assert(c.wallAt(Location::Top) == &bc);
	assert(b.wallAt(Location::Bottom) == &bc);
}


void checkInitializeMaze() {
	Maze<3, 3> maze;
	maze.init();

	auto cells = maze.grid;

	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			Cell cell = cells[i][j];
			assert(cell.id.row == i);
			assert(cell.id.col == j);
			assert(cell.walls.size() == 4);
		}
	}

	

	Cell cell00 = cells[0][0];
	assert(cell00.neighbours.size() == 2);
	assert(cell00.wallAt(Location::Left)->right == nullptr);
	assert(cell00.wallAt(Location::Bottom)->right == nullptr);
	assert(cell00.wallAt(Location::Right)->borders(&cells[0][1]));
	assert(cell00.wallAt(Location::Top)->borders(&cells[1][0]));

	Cell cell01 = cells[0][1];
	assert(cell01.wallAt(Location::Bottom)->right == nullptr);
	assert(cell01.wallAt(Location::Left)->borders(&cell00));
	assert(cell01.wallAt(Location::Right)->borders(&cells[0][2]));
	assert(cell01.wallAt(Location::Top)->borders(&cells[1][1]));

	Cell cell02 = cells[0][2];
	assert(cell02.wallAt(Location::Bottom)->right == nullptr);
	assert(cell02.wallAt(Location::Right)->right == nullptr);
	assert(cell02.wallAt(Location::Top)->borders(&cells[1][2]));
	assert(cell02.wallAt(Location::Left)->borders(&cells[0][1]));

	Cell cell10 = cells[1][0];
	assert(cell10.wallAt(Location::Left)->borders(noCell));
	assert(cell10.wallAt(Location::Bottom)->borders(&cells[0][0]));
	assert(cell10.wallAt(Location::Right)->borders(&cells[1][1]));
	assert(cell10.wallAt(Location::Top)->borders(&cells[2][0]));

	Cell cell11 = cells[1][1];
	assert(cell11.neighbours.size() == 4);
	assert(cell11.wallAt(Location::Top)->borders(&cells[2][1]));
	assert(cell11.wallAt(Location::Right)->borders(&cells[1][2]));
	assert(cell11.wallAt(Location::Bottom)->borders(&cells[0][1]));
	assert(cell11.wallAt(Location::Left)->borders(&cells[1][0]));

	Cell cell12 = cells[1][2];
	assert(cell12.wallAt(Location::Top)->borders(&cells[2][2]));
	assert(cell12.wallAt(Location::Right)->borders(noCell));
	assert(cell12.wallAt(Location::Bottom)->borders(&cells[0][2]));
	assert(cell12.wallAt(Location::Left)->borders(&cells[1][1]));	
	
	Cell cell20 = cells[2][0];
	assert(cell20.wallAt(Location::Top)->borders(noCell));
	assert(cell20.wallAt(Location::Right)->borders(&cells[2][1]));
	assert(cell20.wallAt(Location::Bottom)->borders(&cells[1][0]));
	assert(cell20.wallAt(Location::Left)->borders(noCell));


	Cell cell21 = cells[2][1];
	assert(cell21.wallAt(Location::Top)->borders(noCell));
	assert(cell21.wallAt(Location::Right)->borders(&cells[2][2]));
	assert(cell21.wallAt(Location::Bottom)->borders(&cells[1][1]));
	assert(cell21.wallAt(Location::Left)->borders(&cells[2][0]));

	Cell cell22 = cells[2][2];
	assert(cell22.wallAt(Location::Top)->borders(noCell));
	assert(cell22.wallAt(Location::Right)->borders(noCell));
	assert(cell22.wallAt(Location::Bottom)->borders(&cells[1][2]));
	assert(cell22.wallAt(Location::Left)->borders(&cells[2][1]));

}

void testGenerator() {
	
	Maze<3, 3> maze;
	Id path[9]{ {0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}, {1, 1} };

	int i = 0;
	RecursiveBackTrackingMazeGenerator generator([&](std::vector<Cell*> cells) {
		auto id = path[++i];
		auto itr = std::find_if(cells.begin(), cells.end(), [&](Cell* cell) {
			return cell->id.row == id.row && cell->id.col == id.col;
		});
		return *itr;
	});

	generator.generate(maze);

	//auto walls = maze.grid[0][0].walls;

	for (int i = 0; i < 8; i++) {
		auto a = maze.cellAt(path[i]);
		auto b = maze.cellAt(path[i + 1]);
		assert(!a->hasWallBetween(b));
	}

	assert(maze.cellAt({ 0, 0 })->hasWallBetween(maze.cellAt({ 0, 1 })));
	assert(maze.cellAt({ 1, 0 })->hasWallBetween(maze.cellAt({ 1, 1 })));
	assert(maze.cellAt({ 2, 1 })->hasWallBetween(maze.cellAt({ 1, 1 })));
	assert(maze.cellAt({ 1, 2})->hasWallBetween(maze.cellAt({ 1, 1 })));
	assert(maze.cellAt({ 0, 1 })->wallAt(Location::Bottom)->borders(noCell));
}

void MazeTestSuit() {
	checkRemoveWall();
	checkWallBetween();
	checkHasWallBetweenNeighbours();
	checkWallAt();
	checkInitializeMaze();
	testGenerator();
}

int main() {
	auto scene = new MazeScene;
	start(scene);
	return 0;
	MazeTestSuit();
}