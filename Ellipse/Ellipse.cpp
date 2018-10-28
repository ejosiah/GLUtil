#include <iostream>

using namespace std;

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "EllipseScene.h"

int main() {
	auto scene = new EllipseScene({ 1000, 1000 });
	return start(scene);
}