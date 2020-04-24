#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "FrameBufferScene.h"

int main() {
	auto scene = new FrameBufferScene;
	start(scene);
	return 0;
}