#pragma once

#include <limits>
#include <utility>
#include <glm/glm.hpp>
#include "../../glm/vec_util.h"

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

			Plane() = default;

			Plane(glm::vec3 n, float d):n(glm::normalize(n)), d(d){}

			Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
				n = glm::normalize(glm::cross(b - a, c - a));
				d = dot(n, a);
			}

			static Plane Null() {
				auto inf = std::numeric_limits<float>::infinity();
				return { {inf, inf, inf}, inf };
			}

		};

		inline bool operator==(const Plane& lhs, const Plane& rhs) {
			using namespace std::rel_ops;
			if (lhs.d != rhs.d) return false;
			if (lhs.n != rhs.n) return false;
			return true;
		}
	}

	inline glm::vec3 projectToPlane(const glm::vec3& p, const geom::Plane& plane, float offset = 0.0f) {
		float t = glm::dot(plane.n, p) - (plane.d+offset);
		return p - t * plane.n;
	}
}