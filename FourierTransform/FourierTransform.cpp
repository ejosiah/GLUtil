#include <iostream>

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "FourierScene.h"

int main() {
	start(new FourierScene);
	return 0;
}