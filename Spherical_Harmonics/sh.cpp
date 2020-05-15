#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "SphericalHarmonicsScene.h"

int main() {
	auto scene = new SphericalHarmonicsScene;
	start(scene);
	return 0;
}