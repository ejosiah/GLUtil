#pragma once
#include "triangle_ray_intersection.h"
#include <iostream>



class LineIntersectsTriangle : public TriangleIntersectionTest {
public:
	LineIntersectsTriangle() :
		TriangleIntersectionTest("Line L passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_Line_intersect.vert", make_tuple(&TestData[0], size), 1.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> LineIntersectsTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.34f, 1.6f, -2.7f, 1.0f } ,{ 0.29f, -1.5f, -2.0f, 1.0f } };
GLuint LineIntersectsTriangle::size = sizeof(float) * 20;


class LineDoesNotIntersectsLeftTriangle : public TriangleIntersectionTest {
public:
	LineDoesNotIntersectsLeftTriangle() :
		TriangleIntersectionTest("Line L in the left direction does not passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_Line_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> LineDoesNotIntersectsLeftTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.3f, 1.7f, 3.9f, 1.0f },{ -2.5f, -0.11f, 1.7f, 1.0f } };
GLuint LineDoesNotIntersectsLeftTriangle::size = sizeof(float) * 20;

class LineDoesNotIntersectsRightTriangle : public TriangleIntersectionTest {
public:
	LineDoesNotIntersectsRightTriangle() :
		TriangleIntersectionTest("Line L in the right direction does not passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_Line_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> LineDoesNotIntersectsRightTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.71f, 1.9f, -4.3f, 1.0f },{ 6.7f, -2.3f, 0.21f, 1.0f } };
GLuint LineDoesNotIntersectsRightTriangle::size = sizeof(float) * 20;

class CoplanarLineAndTriangleDoNotIntersect : public TriangleIntersectionTest {
public:
	CoplanarLineAndTriangleDoNotIntersect() :
		TriangleIntersectionTest("Line and Triangle on the same plane do not intersect", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_Line_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> CoplanarLineAndTriangleDoNotIntersect::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.71f, 0.0f, -4.3f, 1.0f },{ 6.7f, 0.0f, 0.21f, 1.0f } };
GLuint CoplanarLineAndTriangleDoNotIntersect::size = sizeof(float) * 20;

class LineOnTriangleDirectionButNotIntersecting : public TriangleIntersectionTest {
public:
	LineOnTriangleDirectionButNotIntersecting() :
		TriangleIntersectionTest("Line on path to intersect Triangle ABC but Not passing through should not Intersect", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_Line_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}



private:
	static vector<vec4> TestData;
	static GLuint size;
};

vector<vec4> LineOnTriangleDirectionButNotIntersecting::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ -0.21f, 4.1f, -4.3f, 1.0f }, {0.11f, 2.0f, -1.3f, 1.0f} }; // { 0.11f, 0.74f, -1.3f, 1.0f } };
GLuint LineOnTriangleDirectionButNotIntersecting::size = sizeof(float) * 20;