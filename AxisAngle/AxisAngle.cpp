// AxisAngle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#define DEBUG

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "AxisAngleScene.h"

int main()
{
	Scene* scene = new AxisAngleScene;
	return start(scene);
}

