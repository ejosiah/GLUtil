
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "LayeringScene.h"

int main() {
	auto scene = new LayeringScene;
	start(scene);
	return 0;
}