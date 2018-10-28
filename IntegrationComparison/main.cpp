#include <iostream>

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "IntegrationComparisonSCene.h"

int main() {
	auto scene = new IntegrationComparisonScene;
	start(scene);
}