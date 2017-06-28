// GLUtil.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include "stdafx.h"

#ifndef CONNECT_3D
#define CONNECT_3D
#endif

#include "include/ncl/gl/GlfwApp.h"
#include "TestScene.h"
#include "exampleScene.h"
#include "include/ncl/gl/Keyboard.h"
#include "ImageViewer.h"
#include <cstdlib>
#include "include/ncl/gl/logger.h"
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

	Options ops;
	ops.hideCursor = true;
	ops.requireMouse = true;
	ops.useDefaultShader = true;
	
	GLVersion version{ 4, 5 };
//	Scene* scene = new ImageViewer();
	Scene* scene = new TestScene("Test Scene", ops);
 //   Scene* scene = new ExampleScene(ops);
//	Scene* scene = new FontTest();
	GlfwApp app(*scene, version);
	app.run();
	delete scene;
    return 0;
		
}

