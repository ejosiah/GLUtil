#pragma once
#include "Test.h"

class TriangleIntersectionTest : public Test {
public:
	TriangleIntersectionTest(string msg, string path, tuple<void*, GLuint> data, float expected) :Test(msg, path, data, expected) {}

	void afterTest() override {
		glBindBuffer(GL_ARRAY_BUFFER, extraData->buffer());
		vec4 v = *(vec4*)glMapNamedBuffer(extraData->buffer(), GL_READ_ONLY);
		cout << msg << ": ";
		cout << "u: " << v.x << ", v: " << v.y << ", w: " << v.z << ", t: " << v.w << endl;

		glUnmapNamedBuffer(extraData->buffer());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};

class RayIntersectsTriangle : public TriangleIntersectionTest {
public:
	RayIntersectsTriangle() :
		TriangleIntersectionTest("Ray R passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_ray_intersect.vert", make_tuple(&TestData[0], size), 1.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> RayIntersectsTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.34f, 1.6f, -2.7f, 1.0f } ,{ 0.29f, -1.5f, -2.0f, 1.0f } };
GLuint RayIntersectsTriangle::size = sizeof(float) * 20;


class RayDoesNotIntersectsLeftTriangle : public TriangleIntersectionTest {
public:
	RayDoesNotIntersectsLeftTriangle() :
		TriangleIntersectionTest("Ray R in the left direction does not passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_ray_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> RayDoesNotIntersectsLeftTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.3f, 1.7f, 3.9f, 1.0f },{ -2.5f, -0.11f, 1.7f, 1.0f } };
GLuint RayDoesNotIntersectsLeftTriangle::size = sizeof(float) * 20;

class RayDoesNotIntersectsRightTriangle : public TriangleIntersectionTest {
public:
	RayDoesNotIntersectsRightTriangle() :
		TriangleIntersectionTest("Ray R in the right direction does not passes through triangle ABC", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_ray_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> RayDoesNotIntersectsRightTriangle::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.71f, 1.9f, -4.3f, 1.0f },{ 6.7f, -2.3f, 0.21f, 1.0f } };
GLuint RayDoesNotIntersectsRightTriangle::size = sizeof(float) * 20;

class CoplanarRayAndTriangleDoNotIntersect : public TriangleIntersectionTest {
public:
	CoplanarRayAndTriangleDoNotIntersect() :
		TriangleIntersectionTest("Ray and Triangle on the same plane do not intersect", "D:\\Users\\Josiah\\documents\\visual studio 2015\\Projects\\GLUtil\\GLSLTest\\triangle_ray_intersect.vert", make_tuple(&TestData[0], size), 0.0f) {

	}

private:
	static vector<vec4> TestData;
	static GLuint size;
};


vector<vec4> CoplanarRayAndTriangleDoNotIntersect::TestData = vector<vec4>{ vec4(0),{ 4.0f, 0.0f, -4.0f, 1.0f },{ -4.0f, 0.0f, -4.0f, 1.0f },{ 0.71f, 0.0f, -4.3f, 1.0f },{ 6.7f, 0.0f, 0.21f, 1.0f } };
GLuint CoplanarRayAndTriangleDoNotIntersect::size = sizeof(float) * 20;