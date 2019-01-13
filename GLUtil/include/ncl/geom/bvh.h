#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <utility>
#include <stack>
#include <limits>
#include <string>
#include <iostream>
#include <sstream>
#include "../data_structure/binary_tree.h"
#include "../../glm/vec_util.h"
#include "../geom/Plane.h"
#include <gl/gl_core_4_5.h>
#include <algorithm>
#include <tuple>

namespace ncl {
	namespace geom {
		namespace bvh {

			static void log(std::string msg) {
				std::cout << msg << std::endl;
			}

			struct Triangle {
				unsigned id;
				glm::vec3 a;
				glm::vec3 b;
				glm::vec3 c;
			};

			using Triangles = std::vector<Triangle>;
			using TriIterator = Triangles::iterator;

#pragma pack (push, 1)
			struct Box {
				glm::vec4 min = glm::vec4(std::numeric_limits<float>::max());
				glm::vec4 max = glm::vec4(std::numeric_limits<float>::lowest());
			};
#pragma pack (pop)

			struct BVHNode {
				Box box;
				BVHNode* child[2];
				std::vector<Triangle> triangles;
			};


#pragma pack (push, 1)
			struct LBVHNode {
				Box box;
				int id;
				int contentOffset;
				int size;
				int isLeaf;
				int child[2];
			};
#pragma pack (pop)

			struct BVH_SSO {
				std::vector<LBVHNode> nodes;
			};

			struct BVH_TRI_INDEX {
				std::vector<int> data;
			};

			Box enclose(const std::vector<Triangle>& triangles) {
				glm::vec3 min;
				glm::vec3 max;
				min = max = triangles.front().a;


				for (size_t i = 0; i < triangles.size(); i++) {
					auto t = triangles[i];

					if (i != 0) {
						min = glm::min(min, t.a);
						max = glm::max(max, t.a);
					}

					min = glm::min(min, t.b);
					min = glm::min(min, t.c);


					max = glm::max(max, t.b);
					max = glm::max(max, t.c);
				}
				return Box{ glm::vec4(min, 0), glm::vec4(max, 0) };
			}

			Box enclose2(TriIterator triangles, TriIterator end) {
				glm::vec3 min;
				glm::vec3 max;
				min = max = triangles->a;

				auto first = triangles;

				for (; triangles != end; triangles++) {
					auto t = *triangles;

					if (triangles != first) {
						min = glm::min(min, t.a);
						max = glm::max(max, t.a);
					}

					min = glm::min(min, t.b);
					min = glm::min(min, t.c);


					max = glm::max(max, t.b);
					max = glm::max(max, t.c);
				}
				return Box{ glm::vec4(min, 0), glm::vec4(max, 0) };
			}

			// TODO move to vec_util or check glm for similar function
			glm::vec3 larger(const glm::vec3& a, const glm::vec3& b) {
				return a < b ? b : a;
			}

			Plane bestSplitPlane(const Box& box) {
				using namespace glm;
				vec3 x = vec3(box.max.x, box.min.y, box.min.z) - box.min.xyz;
				vec3 y = vec3(box.min.x, box.max.y, box.min.z) - box.min.xyz;
				vec3 z = vec3(box.min.x, box.min.y, box.max.z) - box.min.xyz;

				vec3 n = normalize(larger(x, larger(y, z)));
				vec3 X = 0.5f * (box.min.xyz + box.max.xyz);
				Plane p;
				p.n = n;
				p.d = dot(n, X);
				return p;
			}


			BVHNode* create(glm::vec4* geometry, size_t num_tris, size_t max_depth, size_t min_tri_per_node) {
				assert(min_tri_per_node > 0);
				assert(max_depth > 0);

				using namespace std;

				std::function<std::pair<Triangles, Triangles>(const Triangles&, const Box&)> split = [](const Triangles& triangles, const Box& box) {
					geom::Plane p = bestSplitPlane(box);

					//		log("best splitting plane: [" + to_string(p.d) +  ", n:{" + to_string(p.n.x) + "," + to_string(p.n.y) + "," + to_string(p.n.z) + "}]");

					Triangles left, right;

					for (auto& t : triangles) {
						auto d0 = glm::dot(p.n, t.a);
						auto d1 = glm::dot(p.n, t.b);
						auto d2 = glm::dot(p.n, t.c);

						if (d0 > p.d || d1 > p.d || d2 > p.d) {
							left.push_back(t);
						}
						else {
							right.push_back(t);
						}

					}

					return std::make_pair(left, right);
				};

				std::function<BVHNode*(Triangles&, size_t)> buildTree = [&](Triangles& triangles, size_t depth) {
					using namespace std;
					if (triangles.empty()) return (BVHNode*)nullptr;

					if (depth >= max_depth || triangles.size() <= min_tri_per_node) {
					//	log("reached leaf node, depth: " + to_string(depth) + " storing " + to_string(triangles.size()) + " triangles");
						auto node = new BVHNode;
						node->box = enclose(triangles);
						node->child[0] = node->child[1] = nullptr;
						node->triangles = triangles;
						return node;
					}

					Box box = enclose(triangles);

					auto pair = split(triangles, box);

					/*
					log("bounding box: min[" + to_string(box.min.x) + ", " + to_string(box.min.y) + ", " + to_string(box.min.z) + "]");
					log("bounding box: max[" + to_string(box.max.x) + ", " + to_string(box.max.y) + ", " + to_string(box.max.z) + "]");
					log("currently at parent node, depth: " + to_string(depth) + " splitting " + to_string(triangles.size()) + " between child");
					log(to_string(pair.first.size()) + " triangles to left child and " + to_string(pair.second.size()) + " to right child\n\n");*/


					auto node = new BVHNode;
					node->box = box;
					node->child[0] = buildTree(pair.first, depth + 1);
					node->child[1] = buildTree(pair.second, depth + 1);

					return node;
				};

				std::vector<Triangle> triangles;
				auto min = glm::vec3(numeric_limits<float>::max());
				auto max = glm::vec3(numeric_limits<float>::min());
				for (int i = 0; i < num_tris; i++) {
					using namespace glm;
					Triangle t;

					t.id = i;
					t.a = (*(geometry + i * 3)).xyz;
					t.b = (*(geometry + i * 3 + 1)).xyz;
					t.c = (*(geometry + i * 3 + 2)).xyz;

					triangles.push_back(std::move(t));
				}

				return buildTree(triangles, 0);
			};


			template<typename Node>
			void dispose(Node* node) {
				std::function<void(Node*)> free = [](Node* n) { delete n;  };
				postOrder(node, free);
			}

			void updateIndex(BVHNode* node, LBVHNode& lnode, BVH_TRI_INDEX& bvi_index) {
				if (!node->triangles.empty()) {
					lnode.contentOffset = bvi_index.data.size();
					lnode.size = node->triangles.size();
					for (auto& t : node->triangles) {
						bvi_index.data.push_back(t.id);
					}
				}
			}

			void buildLBVH(BVHNode* node, BVH_SSO& sso, BVH_TRI_INDEX& bvi_index) {
				if (nullptr == node) return;
				int id = -1;
				std::function<int()> nextId = [&id]() { return ++id; };

				std::function<BVHNode*(std::stack<std::pair<BVHNode*, LBVHNode>>)> peek = [](std::stack<std::pair<BVHNode*, LBVHNode>> stack) {
					if (stack.empty()) return (BVHNode*)nullptr;
					return stack.top().first;
				};

				std::function<BVHNode*(std::stack<BVHNode*>)> pop = [](std::stack<BVHNode*> stack) {
					auto n = stack.top();
					stack.pop();
					return n;
				};

				std::function<LBVHNode(BVHNode*)> create = [&](BVHNode* node) {
					LBVHNode lnode;
					if (node == nullptr) return lnode;
					lnode.box = node->box;
					lnode.id = nextId();
					lnode.child[0] = lnode.child[1] = -1;
					lnode.contentOffset = -1;
					lnode.size = 0;
					lnode.isLeaf = (nullptr == node->child[0] && nullptr == node->child[1]);
					updateIndex(node, lnode, bvi_index);
					return lnode;
				};

				std::stack<std::pair<BVHNode*, LBVHNode>> stack;

				std::pair<BVHNode*, LBVHNode> curr_pair = std::make_pair(node, create(node));
				auto trigger = node->child[0];
				do {

					while (curr_pair.first != nullptr) {
						LBVHNode& parent = curr_pair.second;

						if (nullptr != curr_pair.first->child[1]) {
							LBVHNode rightChild = create(curr_pair.first->child[1]);
							parent.child[1] = rightChild.id;
							stack.push(std::make_pair(curr_pair.first->child[1], rightChild));
						}
						stack.push(curr_pair);
						LBVHNode leftChild = create(curr_pair.first->child[0]);
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

			void buildLinearBVH(BVHNode& bvh, BVH_SSO& sso, BVH_TRI_INDEX& bvi_index) {
				auto size = ncl::ds::tree::size(&bvh);
				sso.nodes.resize(size);

				buildLBVH(&bvh, sso, bvi_index);
			}

			void send_ssbo(GLuint& buffer, GLuint bindPoint, GLsizei size, void* data = NULL) {
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
				glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindPoint, buffer);
			}
		}
	}
}