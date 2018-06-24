#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {


		/**
		*	point-normal form representation of a plane with the implicit equation
		*	n . X = n . P
		*	
		*/
		struct Plane {
			glm::vec3 n; // plane normal
			float d;	// distance from origin

			Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
				n = glm::normalize(glm::cross(b - a, c - a));
				d = dot(n, a);
			}

		};
	}
}