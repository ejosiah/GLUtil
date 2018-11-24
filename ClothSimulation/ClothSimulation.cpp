// ClothSimulation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#define DEBUG

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "ClothSimulationScene.h"


int main()
{
	Scene* scene = new ClothSimScene;
	return start(scene);
}

