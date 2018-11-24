
#define DEBUG

#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "SliderScene.h"

int main()
{
	Scene* scene = new SliderScene;
	return start(scene);
}
