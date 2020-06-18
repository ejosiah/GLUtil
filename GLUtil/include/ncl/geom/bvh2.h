#pragma once

#include <memory>
#include <vector>
#include <limits>
#include "aabb2.h"

namespace ncl {
	namespace geom {
		namespace bvh {

			struct BVHStats {
				size_t height;
				size_t nodes;
				float primsPerNode;
			};

			const int INT_MAX_VALUE = std::numeric_limits<int>::max();

			enum PrimitiveType { TRIANGLE, SPHERE, BOX, PLANE, BVH };
			enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

			struct Primitive;
			struct BVHBuildNode;
			struct BVHPrimitiveInfo;
			struct LinearBVHNode;

			struct BVH_SSO {
				std::vector<LinearBVHNode> nodes;
			};

			struct BVH_TRI_INDEX {
				std::vector<int> data;
			};

			class BVHBuilder {
			public:
				BVHBuilder(const std::vector<Primitive> &prims, int maxDepth = 7, SplitMethod sMethod = SplitMethod::SAH);

				void buildLinearBVH(BVHBuildNode* bvh, BVH_SSO& sso, BVH_TRI_INDEX& bvi_index, int rootIdx = 0);
			
			protected:
				BVHBuildNode* recursiveBuild(std::vector<BVHPrimitiveInfo>& primitiveInfo, int start, int end, int *totalNodes, int depth, std::vector<Primitive> &orderedPrims);

				void updateIndex(BVHBuildNode* node, LinearBVHNode& lnode, BVH_TRI_INDEX& bvi_index);

			private:
				int maxDepth;
				int maxPrimitivesPerNode;
				const SplitMethod splitMethod;
			public:
				std::vector<Primitive> primitives;
				int totalNodes;
				BVHBuildNode* root;
				
			};

			template<typename Object, typename GetBounds>
			BVHBuildNode* build(BVHBuildNode* bvhRoot, Object* objPtr, int count, GetBounds getBounds,  BVH_TRI_INDEX& bvh_index, BVH_SSO& bvh_ssbo, int maxDepth = 8, int startId = 0, int rootIdx = 0) {
				std::vector<Primitive> primitives;

				for (int i = startId; i < (startId + count); i++) {
					Primitive p;
					Object obj = *(objPtr + i);
					p.id = i;
					p.bounds = getBounds(p.bounds, obj);
					primitives.push_back(p);
				}


				BVHBuilder bvhBuilder{ primitives, maxDepth };
				auto root = bvhBuilder.root;
				bvhBuilder.buildLinearBVH(root, bvh_ssbo, bvh_index, rootIdx);
				bvhRoot = bvhBuilder.root;
				return root;
			}
		}
	}
}

#include "detail/bvh2.inl"