#pragma once
#include <vector>
#include <unordered_map>
#include <tuple>
#include <utility>

namespace ncl {
	namespace gl {

		struct Face;

		struct HalfEdge {
			unsigned int vert;
			HalfEdge* pair = nullptr;
			Face* face = nullptr;
			HalfEdge* next = nullptr;
			unsigned id;
		};

		struct Face {
			HalfEdge* edge;
		};

		std::tuple<std::vector<Face*>, std::unordered_map<unsigned, HalfEdge*>>  buildEdges(std::vector<unsigned>& indices) {
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
			return make_tuple(faces, edges);
		}
	}
}