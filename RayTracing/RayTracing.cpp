#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "RayTracingScene.h"

int main() {
	auto scene = new RayTracingScene;
	start(scene);
	return 0;
}