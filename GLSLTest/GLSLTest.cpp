// GLSLTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DEBUG_MODE

#include <ncl/configured_logger.h>
#include <ncl/gl/GlfwApp.h>
#include "TestRunner.h"
#include "PointShouldBeInTriangleTest.h"
#include "triangle_ray_intersection.h"
#include "line_triangle_intersection.h"

int main()
{

	vector<Test*> tests = {
		new PointShouldBeInTriangleTest,
		
		new PointShouldBeOutSizeTriangleTest,
		new RayIntersectsTriangle,
		new RayDoesNotIntersectsLeftTriangle,
		new RayDoesNotIntersectsRightTriangle,
		new CoplanarRayAndTriangleDoNotIntersect,

		new LineIntersectsTriangle,
		new LineDoesNotIntersectsLeftTriangle,
		new LineDoesNotIntersectsRightTriangle,
		new CoplanarLineAndTriangleDoNotIntersect,
		new LineOnTriangleDirectionButNotIntersecting
	};

	TestRunner* runner = new TestRunner(tests);
	return start(runner);
}

