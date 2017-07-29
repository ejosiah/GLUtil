#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "logger.h"
#include "HalfEdge.h"

namespace ncl {
	namespace gl {
		class WithTriangleAdjacency {
		public:
			std::vector<unsigned> addAdjacency(std::vector<unsigned>& indices) {
				using namespace std;
				auto result = buildEdges(indices);

				vector<Face*> faces = get<0>(result);
				unordered_map<unsigned, HalfEdge*> edges = get<1>(result);

				int boundaries = 0;
				int matchingEdges = 0;
				for (auto face : faces) {
					auto edge = face->edge;
					for (int i = 0; i < 3; i++) {
						unsigned long id = ((edge->id & 0xFFFF) << 16) | (edge->id >> 16);
						auto itr = edges.find(id);
						if (itr != edges.end()) {
							edge->pair = itr->second;
							itr->second->pair = edge;
							matchingEdges++;
						}
						else {
							boundaries++;
						}
						edge = edge->next;
					}
				}

				if (boundaries > 0) logger.warn("Mesh is not watertight. Contains " + to_string(boundaries) + " boundary edges");
				logger.info(to_string(matchingEdges) + " matching edges found");

				vector<unsigned> indicesWithAdjacency;

				for (auto face : faces) {
					auto e0 = face->edge;
					auto e1 = e0->next;
					auto e2 = e1->next;

					indicesWithAdjacency.push_back(e0->vert);
					indicesWithAdjacency.push_back(e1->pair ? e1->pair->next->vert : e0->vert);
					indicesWithAdjacency.push_back(e1->vert);
					indicesWithAdjacency.push_back(e2->pair ? e2->pair->next->vert : e1->vert);
					indicesWithAdjacency.push_back(e2->vert);
					indicesWithAdjacency.push_back(e0->pair ? e0->pair->next->vert : e2->vert);
				}

				for (auto face : faces) {
					auto e0 = face->edge;
					auto e1 = e0->next;
					auto e2 = e1->next;

					delete e0;
					delete e1;
					delete e2;
					delete face;
				}

				return indicesWithAdjacency;
			}



		private:
			ncl::Logger logger = ncl::Logger::get("WithTriangleAdjacency");
		};
	}
}