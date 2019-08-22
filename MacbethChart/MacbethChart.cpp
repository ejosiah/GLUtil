// MacbethChart.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "MacBethChartScene.h"

int main() {
	auto scene = new MacBethChartScene;
	start(scene);
}

