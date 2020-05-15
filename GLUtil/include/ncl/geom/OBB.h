#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {
		namespace bvol {
			struct OBB {
				glm::vec3 center;
				glm::mat4 orientation;
				glm::vec3 halfSizes;
			};
		}
	}
}