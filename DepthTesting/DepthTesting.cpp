#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "DepthTestScene.h"

int main() {
	auto scene = new DepthTestScene;
	return start(scene);
}