#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "CloudScene.h"

int main() {
	auto scene = new CloudScene;
	return start(scene);
}