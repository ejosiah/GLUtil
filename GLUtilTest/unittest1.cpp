#include "stdafx.h"

#define GLM_SWIZZLE 

#include "CppUnitTest.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include "../GLUtil/include/ncl/geom/AABB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace ncl::geom;
using namespace bvol;

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
			

			Triangle t{ {-2, -6, 0}, {4, -2, 0}, {-4, 4, 0} };
			glm::vec3 p{ -2, -2, 0 };

			Assert::IsTrue(t.contains(p));

		}

		TEST_METHOD(BaryCenterOfAPointOutSideATriangle)
		{
			// TODO: Your test code here
			using namespace ncl::geom;

			Triangle t{ { -2, -6, 0 },{ 4, -2, 0 },{ -4, 4, 0 } };
			glm::vec3 p{ -6, -2, 0 };
			Assert::IsFalse(t.contains(p));

			p = glm::vec3{ 5, -6, 0.0f };
			Assert::IsFalse(t.contains(p));

		}

		TEST_METHOD(Construction_Of_AABB) {
			glm::vec3 vertices[8] = {{-2, -2, -2}, {-2, -2, 2}, { 2, -2, -2}, {2, -2, 2}, {-2, 2, -2}, {-2, 2, 2}, {2, 2, -2}, {2, 2, 2} };

			AABB aabb{ vertices, 8 };

			Assert::AreEqual(aabb.min().x, vertices[0].x);
			Assert::AreEqual(aabb.min().y, vertices[0].y);
			Assert::AreEqual(aabb.min().z, vertices[0].z);

			Assert::AreEqual(aabb.max().x, vertices[7].x);
			Assert::AreEqual(aabb.max().y, vertices[7].y);
			Assert::AreEqual(aabb.max().z, vertices[7].z);
		}

		TEST_METHOD(AABBs_DO_NOT_INTERSECT) {
			glm::vec3 vertices0[8] = { { -2, -2, -2 },{ -2, -2, 2 },{ 2, -2, -2 },{ 2, -2, 2 },{ -2, 2, -2 },{ -2, 2, 2 },{ 2, 2, -2 },{ 2, 2, 2 } };
			glm::vec3 vertices1[8];
			
			glm::mat4 mat = glm::translate(glm::mat4(1), { 5, 0, 0 });
			for (int i = 0; i < 8; i++) {
				vertices1[i] = (mat * glm::vec4{ vertices0[i], 1.f }).xyz;
			}

			AABB a{ vertices0, 8 };
			AABB b{ vertices1, 8 };

			Assert::IsFalse(a.test(b));
		}


		TEST_METHOD(AABBs_DO_INTERSECT) {
			glm::vec3 vertices0[8] = { { -2, -2, -2 },{ -2, -2, 2 },{ 2, -2, -2 },{ 2, -2, 2 },{ -2, 2, -2 },{ -2, 2, 2 },{ 2, 2, -2 },{ 2, 2, 2 } };
			glm::vec3 vertices1[8];

			glm::mat4 mat = glm::translate(glm::mat4(1), { 0.5, 0, 0 });
			for (int i = 0; i < 8; i++) {
				vertices1[i] = (mat * glm::vec4{ vertices0[i], 1.f }).xyz;
			}

			AABB a{ vertices0, 8 };
			AABB b{ vertices1, 8 };

			Assert::IsTrue(a.test(b));
		}
	

	};
}