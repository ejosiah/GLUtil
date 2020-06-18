#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include "../aabb2.h"
#include "../../gl/common.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <functional>
#include <utility>
#include <stack>
#include <limits>

namespace ncl {
	namespace geom {
		namespace bvh {

			using Bounds = bvol::AABB2;

			struct Primitive {
				size_t id;
				Bounds bounds;
			};

			using namespace bvol::aabb;


			struct BVHBuildNode {

				void initLeaf(int first, int n, const Bounds& b) {
					firstPrimitiveOffest = first;
					nPrimitives = n;
					bounds = b;
					child[0] = child[1] = nullptr;
				}

				void initInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
					child[0] = c0;
					child[1] = c1;
					c0->leftChild = true;
					bounds = Union(c0->bounds, c1->bounds);
					splitAxis = axis;
					nPrimitives = 0;
				}

				bool isLeaf() {
					return nullptr == child[0] && nullptr == child[1];
				}

				Bounds bounds;
				BVHBuildNode* child[2];
				bool leftChild = false;
				int splitAxis, firstPrimitiveOffest, nPrimitives;
			};

			struct BVHPrimitiveInfo {
				BVHPrimitiveInfo() {}
				BVHPrimitiveInfo(size_t primitiveNum, const Bounds& bounds)
					:primitiveNum(primitiveNum)
					, bounds(bounds)
					, centroid(center(bounds)){}

				size_t primitiveNum;
				Bounds bounds;
				glm::vec3 centroid;
			};

#pragma pack(push, 1)
			struct LinearBVHNode {
				Bounds box;
				int splitAxis;
				int id;
				int contentOffset;
				int size;
				int isLeaf;
				int child[2];
				ncl::padding_4 padding;
			};
#pragma pack(pop)

			struct Bucket {
				int count = 0; 
				Bounds bounds;
			};

			BVHBuilder::BVHBuilder(const std::vector<Primitive> &prims, int maxDepth, SplitMethod sMethod) :
				maxDepth(maxDepth), splitMethod(sMethod), primitives(prims) {

				if (prims.empty()) return;

				auto size = prims.size();

				maxPrimitivesPerNode = size / std::pow(2, maxDepth);

				std::vector<BVHPrimitiveInfo> primitiveInfo(size);
				for (size_t i = 0; i < size; i++) {
					primitiveInfo[i] = { i, primitives[i].bounds };
				}

				totalNodes = 0;
				std::vector<Primitive> orderedPrims;
				orderedPrims.reserve(size);
				root = nullptr;

				root = recursiveBuild(primitiveInfo, 0, size, &totalNodes, 0, orderedPrims);
				primitives.swap(orderedPrims);
			}

			BVHBuildNode* BVHBuilder::recursiveBuild(std::vector<BVHPrimitiveInfo>& primitiveInfo, int start, int end, int *totalNodes, int depth, std::vector<Primitive> &orderedPrims) {
				(*totalNodes)++;
				assert(start != end);
				BVHBuildNode* node = new BVHBuildNode;
				Bounds bounds;
				for (int i = start; i < end; i++) {
					bounds = Union(bounds, primitiveInfo[i].bounds);
				}
				int nPrimitives = end - start;
				if (depth >= maxDepth) {
					int firstPrimOffset = orderedPrims.size();
					for (int i = start; i < end; i++) {
						int primNum = primitiveInfo[i].primitiveNum;
						orderedPrims.push_back(primitives[primNum]);
					}
					node->initLeaf(firstPrimOffset, nPrimitives, bounds);
					return node;
				}
				else {
					Bounds centroidBounds;
					for (int i = start; i < end; i++) {
						centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
					}
					int dim = maxExtent(centroidBounds);

					int mid = (start + end) * 0.5;
					if (centroidBounds.max[dim] == centroidBounds.min[dim]) {	// No volume, this case is almost impossible
						int firstPrimOffset = orderedPrims.size();
						for (int i = start; i < end; i++) {
							int primNum = primitiveInfo[i].primitiveNum;
							orderedPrims.push_back(primitives[primNum]);
						}
						node->initLeaf(firstPrimOffset, nPrimitives, bounds);
						return node;
					}
					else {
						switch (splitMethod)
						{
						case SplitMethod::Middle: {
							float pmid = (centroidBounds.min[dim] + centroidBounds.max[dim]) / 2;
							BVHPrimitiveInfo* midPtr =
								std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1
									, [dim, pmid](const BVHPrimitiveInfo& pi) { return pi.centroid[dim] < pmid; });

							mid = midPtr - &primitiveInfo[0];
							// For lots of prims with large overlapping bounding boxes, this
							// may fail to partition; in that case don't break and fall
							// through
							// to EqualCounts.
							if (mid != start && mid != end) break;
						}
						case SplitMethod::EqualCounts: {
							mid = (start + end) * 0.5;
							std::nth_element(
								&primitiveInfo[start]
								, &primitiveInfo[mid], &primitiveInfo[end - 1] + 1
								, [dim](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b) {return a.centroid[dim] < b.centroid[dim];});
							break;
						}
						case SplitMethod::SAH:
						default: {
							if (nPrimitives <= 2) {
								mid = (start + end) * 0.5;
								std::nth_element(
									&primitiveInfo[start]
									, &primitiveInfo[mid], &primitiveInfo[end - 1] + 1
									, [dim](const BVHPrimitiveInfo& a, const BVHPrimitiveInfo& b) {return a.centroid[dim] < b.centroid[dim];});
							}
							else {
								constexpr int nBuckets = 12;
								Bucket buckets[nBuckets];

								for (int i = start; i < end; i++) {
									int b = nBuckets * offset(centroidBounds, primitiveInfo[i].centroid)[dim];
									if (b == nBuckets) b = nBuckets - 1;
									assert(b >= 0);
									assert(b < nBuckets);
									buckets[b].count++;
									buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
								}

								float cost[nBuckets - 1];
								for (int i = 0; i < nBuckets - 1; i++) {
									Bounds b0, b1;
									int count0 = 0, count1 = 0;
									for (int j = 0; j <= i; j++) {
										b0 = Union(b0, buckets[j].bounds);
										count0 += buckets[j].count;
									}
									for (int j = i + 1; j < nBuckets; j++) {
										b1 = Union(b1, buckets[j].bounds);
										count1 += buckets[j].count;
									}
									cost[i] = 1 + (count0 * sufraceArea(b0) + count1 * sufraceArea(b1)) / sufraceArea(bounds);
								}
								float minCost = cost[0];
								int minCostSplitBucket = 0;
								for (int i = 1; i < nBuckets - 1; i++) {
									if (cost[i] < minCost) {
										minCost = cost[i];
										minCostSplitBucket = i;
									}
								}

								float leafCost = nPrimitives;
								if (leafCost > maxPrimitivesPerNode || leafCost > minCost) {
									BVHPrimitiveInfo* pmid = std::partition(
										&primitiveInfo[start]
										, &primitiveInfo[end - 1] + 1
										, [=](const BVHPrimitiveInfo& pi) {
										int b = nBuckets * offset(centroidBounds, pi.centroid)[dim];
										if (b == nBuckets)  b = nBuckets - 1;
										assert(b >= 0);
										assert(b < nBuckets);
										return b <= minCostSplitBucket;
									}
									);
									mid = pmid - &primitiveInfo[0];
								}
								else {
									int firstPrimOffset = orderedPrims.size();
									for (int i = start; i < end; i++) {
										int primNum = primitiveInfo[i].primitiveNum;
										orderedPrims.push_back(primitives[primNum]);
									}
									node->initLeaf(firstPrimOffset, nPrimitives, bounds);
									return node;
								}
							}
							break;
						}
						}
						node->initInterior(
							dim
							, recursiveBuild(primitiveInfo, start, mid, totalNodes, depth + 1, orderedPrims)
							, recursiveBuild(primitiveInfo, mid, end, totalNodes, depth + 1, orderedPrims));
					}
				}
				
				return node;
			}

			void BVHBuilder::buildLinearBVH(BVHBuildNode* node, BVH_SSO& sso, BVH_TRI_INDEX& bvi_index, int rootIdx) {
				sso.nodes.resize(totalNodes + sso.nodes.size());

				if (nullptr == node) return;
				int id = rootIdx - 1 ;
				std::function<int()> nextId = [&id]() { return ++id; };

				std::function<BVHBuildNode*(std::stack<std::pair<BVHBuildNode*, LinearBVHNode>>)> peek = [](std::stack<std::pair<BVHBuildNode*, LinearBVHNode>> stack) {
					if (stack.empty()) return (BVHBuildNode*)nullptr;
					return stack.top().first;
				};

				std::function<BVHBuildNode*(std::stack<BVHBuildNode*>)> pop = [](std::stack<BVHBuildNode*> stack) {
					auto n = stack.top();
					stack.pop();
					return n;
				};

				std::function<LinearBVHNode(BVHBuildNode*)> create = [&](BVHBuildNode* node) {
					LinearBVHNode lnode;
					if (node == nullptr) return lnode;
					lnode.box = node->bounds;
					lnode.id = nextId();
					lnode.child[0] = lnode.child[1] = -1;
					lnode.contentOffset = -1;
					lnode.size = 0;
					lnode.isLeaf = node->isLeaf();
					lnode.splitAxis = node->splitAxis;
					updateIndex(node, lnode, bvi_index);
					return lnode;
				};

				std::stack<std::pair<BVHBuildNode*, LinearBVHNode>> stack;

				std::pair<BVHBuildNode*, LinearBVHNode> curr_pair = std::make_pair(node, create(node));
				auto trigger = node->child[0];
				do {

					while (curr_pair.first != nullptr) {
						LinearBVHNode& parent = curr_pair.second;

						if (nullptr != curr_pair.first->child[1]) {
							LinearBVHNode rightChild = create(curr_pair.first->child[1]);
							parent.child[1] = rightChild.id;
							stack.push(std::make_pair(curr_pair.first->child[1], rightChild));
						}
						stack.push(curr_pair);
						LinearBVHNode leftChild = create(curr_pair.first->child[0]);
						curr_pair = std::make_pair(curr_pair.first->child[0], leftChild);
					}

					curr_pair = stack.top();
					stack.pop();

					if (nullptr != curr_pair.first->child[1] && peek(stack) == curr_pair.first->child[1]) {
						// traverse right child if not yet traversed
						auto child_pair = stack.top();
						stack.pop();
						stack.push(curr_pair);
						curr_pair = child_pair;
					}
					else {
						if (!stack.empty() && stack.top().first->child[0] == curr_pair.first) { // append left child pos to parent node
							stack.top().second.child[0] = curr_pair.second.id;
						}

						sso.nodes[curr_pair.second.id] = curr_pair.second;
						curr_pair = std::make_pair(nullptr, create(nullptr));
					}

				} while (!stack.empty());
			}

			void BVHBuilder::updateIndex(BVHBuildNode* node, LinearBVHNode& lnode, BVH_TRI_INDEX& bvi_index) {
				if (node->isLeaf()) {
					lnode.contentOffset = bvi_index.data.size();
					lnode.size = node->nPrimitives;

					for (int i = node->firstPrimitiveOffest; i < (node->firstPrimitiveOffest + node->nPrimitives); i++) {
						bvi_index.data.push_back(primitives[i].id);
					}
				}
			}

		}
	}
}