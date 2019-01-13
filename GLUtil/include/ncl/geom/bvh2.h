#pragma once

#include <memory>
#include <vector>
#include <limits>
#include "aabb2.h"

namespace ncl {
	namespace geom {
		namespace bvh {

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

				void buildLinearBVH(BVHBuildNode* bvh, BVH_SSO& sso, BVH_TRI_INDEX& bvi_index);
			
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
		}
	}
}

#include "detail/bvh2.inl"