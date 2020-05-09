
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "PointShadowScene.h"

int main() {
	auto scene = new PointShadowScene;
	start(scene);
	return 0;
}