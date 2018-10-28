// ClothSimulation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "ClothSimulationScene.h"


int main()
{
	Scene* scene = new ClothSimScene;
	return start(scene);
}

