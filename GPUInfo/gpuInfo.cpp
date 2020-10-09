#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/configured_logger.h"
#include "GPUInfoScene.h"

int main() {
	return start(new GPUInfoScene, { 4, 6 });
}