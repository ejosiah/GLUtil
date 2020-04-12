#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "MazeScene.h"

int main() {
	auto scene = new MazeScene;
	start(scene);
	return 0;
}