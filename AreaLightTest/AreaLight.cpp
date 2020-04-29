#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "AreaLightScene.h"

int main() {
	auto scene = new AreaLightScene;
	start(scene);
	return 0;
}