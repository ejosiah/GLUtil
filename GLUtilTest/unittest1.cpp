#include "stdafx.h"

#define GLM_SWIZZLE 

#include "CppUnitTest.h"
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "../GLUtil/include/ncl/geom/Triangle.h"
#include "../GLUtil/include/ncl/geom/AABB.h"
#include "../GLUtil/include/ncl/space_partition/bsp_tree.h"
#include "../GLUtil/include/glm/vec_util.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;
using namespace glm;
using namespace ncl::geom;
using namespace bvol;
using namespace ncl::space_partition;

namespace BaryCenterTest
{		

	TEST_CLASS(UnitTest1)
	{
	public:

		const float EPSILLON = 0.0001f;

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

		BSPTree<vec3>::ClassifyObjectToPlane classifyObjectToPlane = [](vec3 v, Plane p) {
			auto nDotv = dot(p.n, v);
			if (nDotv > 0) return BSPTree<vec3>::Classification::IN_FRONT;
			else if (nDotv < 0) return BSPTree<vec3>::Classification::BEHIND;
			else return BSPTree<vec3>::Classification::COPLANAR;
		};


		TEST_METHOD(BSP_TREE_CREATION) {
			using namespace glm;
			using namespace std::rel_ops;
			
			int count = 0;
			BSPTree<vec3>::PickSplittingPlane pickSplittingPlane = [&](BSPTree<vec3>::BSPObjectSet objs) {
				Plane p;
				p.d = 0;
				switch (count) {
				case 0:
					p.n = { 1, 0, 0 };
					break;
				case 1:
					p.n = { 0, 0, 1 };
					break;
				case 2:
					p.n = { 0, 0, 1 };
					break;
				}
				count++;
				return p;
			};

			vec3 a, b, c, d, e, f, g, h;

			vec3 objs[8] = {
				a = {-1, 0, -1},
				b = {1, 0, -1}, c = {2, 0, -2},
				d = {-1, 0, 1}, e = {-1, 0, 1.5}, f = {-1, 0, 2.0}, g = {-1, 0, 2.5},
				h = {1, 0, 1}
			};
			BSPTree<vec3> tree = BSPTree<vec3>::create(objs, 8, pickSplittingPlane, classifyObjectToPlane, 50, 4);

			Plane p{ {1, 0, 0}, 0 };
			Assert::IsTrue(tree.root().plane() == p,  L"root node plane should be pointing along the x axis");

			Assert::IsTrue(tree.root().front()->isLeaf(), L"root/front node should be a leaf node");			
			Assert::AreEqual(tree.root().front()->objects().size, 3u, L"root/front should have 3 items in it");
			Assert::IsTrue(tree.root().front()->objects().objects[0] == b);
			Assert::IsTrue(tree.root().front()->objects().objects[1] == c);
			Assert::IsTrue(tree.root().front()->objects().objects[2] == h);

			p.n = { 0, 0, 1 };
			Assert::IsFalse(tree.root().back()->isLeaf(), L"root/back node should not be a leaf node");
			Assert::IsTrue(tree.root().back()->plane() == p, L"root/back node plane should be pointing along the z axis");

			Assert::IsTrue(tree.root().back()->front()->isLeaf(), L"root/back/front should be a leaf node");
			Assert::AreEqual(tree.root().back()->front()->objects().size, 4u, L"root/back/front should have 4 items in it");
			Assert::IsTrue(tree.root().back()->front()->objects().objects[0] == d);
			Assert::IsTrue(tree.root().back()->front()->objects().objects[1] == e);
			Assert::IsTrue(tree.root().back()->front()->objects().objects[2] == f);
			Assert::IsTrue(tree.root().back()->front()->objects().objects[3] == g);

			Assert::IsTrue(tree.root().back()->back()->isLeaf(), L"root/back/back should be a leaf node");
			Assert::AreEqual(tree.root().back()->back()->objects().size, 1u, L"root/back/back should have 1 items in it");
			Assert::IsTrue(tree.root().back()->back()->objects().objects[0] == a);

		}

		TEST_METHOD(BSP_TREE_IN_ORDER_TRAVASAL) {
			using namespace glm;
			using namespace std::rel_ops;

			int count = 0;
			BSPTree<vec3>::PickSplittingPlane pickSplittingPlane = [&](BSPTree<vec3>::BSPObjectSet objs) {
				Plane p;
				p.d = 0;
				switch (count) {
				case 0:
					p.n = { 0, 0, -1 };
					break;
				case 1:
				case 2:
					p.n = { -1, 0, 0 };
					break;
				}
				count++;
				return p;
			};

			vec3 a, b, c, d1, d2, e, f, g, h, i, j1, j2, k, l;

			vec3 objects[14]{
				a = {1, 1, 0},
				b = {0, 1, -2},
				c = {-1, 1, -3},
				d1 = {-1, 1, -2},
				d2 = {-1, 1, 2},
				e = {-0.8, 1, 1},
				f = {-0.5, 1, 1},
				g = {-1, 1, 0},
				h = {0, 1, 2},
				i = {1, 1, 1},
				j1 = {1, 1, -1.5},
				j2 = {1, 1, 1.5},
				k = {1, 1, -2},
				l = {0.8, 1, -1.8}
			};

			BSPTree<vec3> tree = BSPTree<vec3>::create(objects, 14, pickSplittingPlane, classifyObjectToPlane, 50, 4, BSPTree<vec3>::Type::NODE_STORING);

			vec3 inOrder[] = { c, d1, b, j1, k, l, a, g, d2, e, f, h, i, j2 };

			auto expected = begin(inOrder);
			auto actual = tree.inOrderIterator();
			while (expected != end(inOrder)) {
				stringstream ss;
				ss << *expected << " == " << *actual;
				Logger::WriteMessage(ss.str().c_str());
				Assert::IsTrue(*expected == *actual);
				++expected;
				++actual;
			}
		}
	

	};
}