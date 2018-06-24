#include "stdafx.h"
#include "CppUnitTest.h"
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/geom/Triangle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BaryCenterTest
{		

	TEST_CLASS(UnitTest1)
	{
	public:

		const float EPSILLON = 0.0001;

		bool isEnclosedInTriangle(float u, float v, float w) {
			return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
		}
		
		TEST_METHOD(BaryCenterOfAPointInSideATriangle)
		{
			// TODO: Your test code here
			using namespace ncl::geom;

			Triangle t{ {-2, -6, 0}, {4, -2, 0}, {-4, 4, 0} };
			glm::vec3 p1{ -2, -2, 0 };

			glm::vec3 weights = barycenter(p1, t);
			float u = weights.x;
			float v = weights.y;
			float w = weights.z;

			Assert::IsTrue(isEnclosedInTriangle(u, v, w));

		}

		TEST_METHOD(BaryCenterOfAPointOutSideATriangle)
		{
			// TODO: Your test code here
			using namespace ncl::geom;

			Triangle t{ { -2, -6, 0 },{ 4, -2, 0 },{ -4, 4, 0 } };
			glm::vec3 p1{ -6, -2, 0 };

			glm::vec3 weights = barycenter(p1, t);
			float u = weights.x;
			float v = weights.y;
			float w = weights.z;

			Assert::IsFalse(isEnclosedInTriangle(u, v, w));

		}

	};
}