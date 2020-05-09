
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "ShadowMapScene2.h"

int main() {
	auto scene = new ShadowMapScene2;
	start(scene);
	return 0;
}