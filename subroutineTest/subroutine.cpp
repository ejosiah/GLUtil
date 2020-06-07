#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "SubroutineScene.h"

int main() {
	auto scene = new SubRoutineScene;
	start(scene);
	return 0;
}