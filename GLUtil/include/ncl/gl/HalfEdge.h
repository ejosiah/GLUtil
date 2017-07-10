#pragma once

namespace ncl {
	namespace gl {
		struct HalfEdge {
			unsigned int vert;
			HalfEdge* pair;
			Face* face;
			HalfEdge* next;
			unsigned id;
		};

		struct Face {
			HalfEdge* edge;
		};
	}
}