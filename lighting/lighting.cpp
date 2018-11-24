#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#define DEBUG

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "LightingScene.h"

int main()
{
	Scene* scene = new LightingScene;
	return start(scene);
}
