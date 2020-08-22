#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "PhysicsScene.h"

int main() {
	auto scene = new PhysicsScene("Physics Scene");
	return start(scene);
}