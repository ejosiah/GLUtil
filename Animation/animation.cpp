#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "AnimationScene.h"

int main() {
	auto scene = new AnimationScene;
	return start(scene);
}

