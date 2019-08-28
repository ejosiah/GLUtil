
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "SamplingScene.h"

int main() {
	auto scene = new SamplingScene;
	start(scene);
	return 0;
}