#pragma once
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "TessellationScene.h"

int main() {
	auto scene = new TessellationScene;
	start(scene);
	return 0;
}