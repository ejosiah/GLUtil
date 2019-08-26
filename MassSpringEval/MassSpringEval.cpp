#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "MassSpringScene.h"

int main() {
	auto scene = new MassSpringScene;
	start(scene);
	return 0;
}