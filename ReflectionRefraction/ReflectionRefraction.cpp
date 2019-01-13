#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#define DEBUG

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "ReflectScene.h"

int main()
{
	Scene* scene = new ReflectRefractScene;
	return start(scene);
}
