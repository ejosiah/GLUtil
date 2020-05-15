#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {
		namespace bvol {
			struct Sphere {
				float radius;
				glm::vec3 center;
			};
		}
	}
}