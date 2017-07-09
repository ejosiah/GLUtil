#pragma once
#include <unordered_map>
#include <vector>
#include "HalfEdge.h"

namespace ncl {
	namespace gl {
		class WithTriangleAdjacency {

			std::vector<unsigned> operator()(std::vector<unsigned>& indices) {
				
				size_t size = indices.size();

				std::unordered_map<unsigned, HalfEdge> edges;

				for (int i = 0; i < size; i += 3) {
					unsigned v0 = indices[i];
					unsigned v1 = indices[i + 1];
					unsigned v2 = indices[i + 2];

					HalfEdge e0;
					HalfEdge e1;
					HalfEdge e2;

					e0.vert = v0;
					e1.vert = v1;
					e2.vert = v2;

					e0.next = &e2;
					e1.next = &e0;
					e2.next = &e1;

					Face f;
					f.edge = &e0;
					e0.face = e1.face = e2.face = &f;

					edges.insert(std::make_pair(v2 | (v0 << 16), e0));
					edges.insert(std::make_pair(v0 | (v1 << 16), e1));
					edges.insert(std::make_pair(v1 | (v2 << 16), e2));
				}
				if (edges.size() != size) {
					throw "Bad mesh:  dupicated edges or inconsistent winding";
				}

				for (auto e : edges) {
					auto index = e.first;
					auto halfEdge0 = e.second;
					unsigned long pairIndex = ((index & 0xFFFF) << 16) | (index >> 16);
					auto halfEdge1 = edges[pairIndex];
					halfEdge0.pair = &halfEdge1;
					halfEdge1.pair = &halfEdge0;
				}
			}
		};
	}
}