#pragma once

#include "../gl/util.h"
#include <glm/glm.hpp>
#include <vector>

namespace ncl {
	namespace sampling {


		inline std::vector<glm::vec3> pointsInSphere(int numPoints) {
			std::vector<glm::vec3> points;
			auto rng = Random{};
			for (int i = 0; i < numPoints; i++) {

				glm::vec3 point;

				do {
					point = rng.vector(glm::vec3(-1), glm::vec3(1));
				} while (glm::dot(point, point) >= 1.0);

				points.push_back(glm::normalize(point));
			}
			return points;
		}

	}
}