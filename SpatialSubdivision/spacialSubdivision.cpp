#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "SpatialSubdivisionScene.h"

int main() {
	auto scene = new SpatialSubdivisionScene;
	return start(scene);
}