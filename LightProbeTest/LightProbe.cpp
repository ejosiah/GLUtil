
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "LightProbScene.h"

int main() {
	auto scene = new LightProbeScene;
	start(scene);
	return 0;
}