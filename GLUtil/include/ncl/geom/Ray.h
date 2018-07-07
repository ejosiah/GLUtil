#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {

		struct Ray {
			glm::vec3 origin;
			glm::vec3 direction;
		};

		struct Line {
			glm::vec3 p0;
			glm::vec3 p1;

			Ray toRay() {
				return { p0, glm::normalize(p1 - p0) };
			}
		};

	};
}