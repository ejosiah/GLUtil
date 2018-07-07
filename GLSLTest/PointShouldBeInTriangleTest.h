#pragma once

#include "Test.h"

/*
Triangle t{ {-2, -6, 0}, {4, -2, 0}, {-4, 4, 0} };
glm::vec3 p{ -2, -2, 0 };
*/

class PointShouldBeInTriangleTest : public Test {
public:
	PointShouldBeInTriangleTest() :
		Test("Point P should be contained within trangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\point_in_triangle.vert", make_tuple(&TestData[0], size), 1.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> PointShouldBeInTriangleTest::TestData = vector<vec4>{ { -2, -6, 0, 0 },{ 4, -2, 0, 0 },{ -4, 4, 0, 0 },{ -2, -2, 0, 0 } };
GLuint PointShouldBeInTriangleTest::size = sizeof(float) * 16;

class PointShouldBeOutSizeTriangleTest : public Test {
public:
	PointShouldBeOutSizeTriangleTest() :
		Test("Point P should be outsize of trangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\point_in_triangle.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};

vector<vec4> PointShouldBeOutSizeTriangleTest::TestData = vector<vec4>{ { -2, -6, 0, 0 },{ 4, -2, 0, 0 },{ -4, 4, 0, 0 }, { -6, -2, 0, 0 } };
GLuint PointShouldBeOutSizeTriangleTest::size = sizeof(float) * 16;

class PointInTriangleTest : public TestSuit {
public:
	PointInTriangleTest() :
		TestSuit("Point in Triangle Test",{
				new PointShouldBeInTriangleTest(),
				new PointShouldBeOutSizeTriangleTest()
			}) {
	}
};