#pragma once

namespace ncl {
	namespace gl {
		struct HalfEdge {
			unsigned int vert;
			HalfEdge* pair;
			Face* face;
			HalfEdge* next;
		};

		struct Face {
			HalfEdge* edge;
		};
	}
}