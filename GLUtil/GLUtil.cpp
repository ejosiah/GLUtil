// GLUtil.cpp : Defines the entry point for the console application.
//


#include <Windows.h>
#include "stdafx.h"

//#ifndef CONNECT_3D
//#define CONNECT_3D
//#endif

#ifndef DEBUG_MODE
#define DEBUG_MODE
#endif

#include "include/ncl/gl/GlfwApp.h"
#include "TestScene.h"
#include "exampleScene.h"
#include "include/ncl/gl/Keyboard.h"
#include "ImageViewer.h"
#include <cstdlib>
#include "include/ncl/gl/logger.h"
#include "include/ncl/gl/DoubleBufferedObj.h"
#include "FontTest.h"

using namespace std;
using namespace ncl;
using namespace gl;

static ConsoleAppender CONSOLE_APPENDER;
static FileAppender FILE_APPENDER("log.txt");


vector<Appender*> Logger::appenders = { &CONSOLE_APPENDER,  &FILE_APPENDER };
const unsigned int Logger::level = LEVEL_DEBUG;

int main()
{
	Resolution res = { 1280, 960 };
	Options ops;
	ops.hideCursor = true;
	ops.requireMouse = false;
	ops.useDefaultShader = false;
	ops.fullscreen = false;
	ops.vSync = false;
	ops.dimensions = { res.width, res.height };
	
	GLVersion version{ 4, 5 };
	Scene* scene = new ImageViewer();
//	Scene* scene = new TestScene("Test Scene", ops);
//    Scene* scene = new ExampleScene(ops);
//	Scene* scene = new FontTest();
	GlfwApp app(*scene, version);
	app.run();
	delete scene;
	
    return 0;
		
}

