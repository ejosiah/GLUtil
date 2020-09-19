#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "VolumetricLightScene.h"

int main() {
	auto scene = new VolumetricLightScene;
	return start(scene);
}