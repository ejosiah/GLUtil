// BarcyCenterExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "BaryCenterScene.h"


int main()
{
	Scene* scene = new BaryCenterScene;
	return start(scene);
}

