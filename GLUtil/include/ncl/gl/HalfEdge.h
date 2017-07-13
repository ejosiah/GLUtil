#pragma once

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
	}
}