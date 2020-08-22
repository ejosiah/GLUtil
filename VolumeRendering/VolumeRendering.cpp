#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "VolumeRenderingScene.h"

int main() {
	auto scene = new VolumeRenderingScene;
	return start(scene);
}