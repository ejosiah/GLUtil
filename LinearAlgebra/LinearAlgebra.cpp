#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "LinearAlgebraScene.h"

int main() {
	auto scene = new LinearAlgebraScene;
	start(scene);
	return 0;
}