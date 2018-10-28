#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "ComputeScene.h"

int main() {
	auto scene = new ComputeScene;
	start(scene);
}