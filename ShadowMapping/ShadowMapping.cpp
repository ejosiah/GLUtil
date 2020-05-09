
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "ShadowMappingScene.h"

int main() {
	auto scene = new ShadowMappingScene;
	start(scene);
	return 0;
}