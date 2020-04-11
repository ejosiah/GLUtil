#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "pbrScene.h"

int main() {
	auto scene = new PbrScene;
	start(scene);
	return 0;
}