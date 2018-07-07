// Orient3D.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "Orient3DScene.h"


int main()
{
	Scene* scene = new Orient3DScene;
	return start(scene);
}

