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
				size_t size = indices.size();

				vector<Face*> faces;
				unordered_map<unsigned, HalfEdge*> edges;

				for (int i = 0; i < size; i += 3) {
					unsigned v0 = indices[i];
					unsigned v1 = indices[i + 1];
					unsigned v2 = indices[i + 2];

					HalfEdge* e0 = new HalfEdge;
					HalfEdge* e1 = new HalfEdge;
					HalfEdge* e2 = new HalfEdge;

					e0->vert = v0;
					e1->vert = v1;
					e2->vert = v2;

					e0->id = v2 | (v0 << 16);
					e1->id = v0 | (v1 << 16);
					e2->id = v1 | (v2 << 16);

					e0->next = e1;
					e1->next = e2;
					e2->next = e0;

					Face* face = new Face;
					face->edge = e0;
					e0->face = e1->face = e2->face = face;

					faces.push_back(face);
					edges.insert(make_pair(e0->id, e0));
					edges.insert(make_pair(e1->id, e1));
					edges.insert(make_pair(e2->id, e2));

				}
				if (edges.size() != size) {
					throw "Bad mesh:  dupicated edges or inconsistent winding";
				}

				int boundaries = 0;
				for (auto face : faces) {
					auto edge = face->edge;
					for (int i = 0; i < 3; i++) {
						unsigned long id = ((edge->id & 0xFFFF) << 16) | (edge->id >> 16);
						auto itr = edges.find(id);
						if (itr != edges.end()) {
							edge->pair = itr->second;
							itr->second->pair = edge;
						}
						else {
							boundaries++;
						}
						edge = edge->next;
					}
				}

				if(boundaries > 0) logger.warn("Mesh is not watertight. Contains " + to_string(boundaries) + " boundary edges");

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