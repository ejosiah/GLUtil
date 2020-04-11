#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "VectorCalculusScene.h"

int main() {
	auto scene = new VectorCalculusScene;
	start(scene);
	return 0;
}