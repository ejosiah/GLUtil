#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace cs {

		constexpr static float PI = 3.1415926535897932384626433;
		constexpr static float HALF_PI = PI / 2;
		constexpr static float TWO_PI = PI * 2;

		class SphericalCoord {
		public:
			float r;
			float theta;
			float phi;


			SphericalCoord(float r = 0, float t = 0, float p = 0) :r(r), theta(t), phi(p) {}

			SphericalCoord(glm::vec3 v) {
				r = glm::length(v);
				theta = glm::acos(v.y / r);
				phi = glm::atan(v.z / v.x);
			}

			void updateTheta(float v) {
				theta += v;
				if (theta < 0) theta += TWO_PI;
				if (theta > TWO_PI) theta -= TWO_PI;
			}

			void updatePhi(float v) {
				phi += v;
				if (phi < 0) phi += TWO_PI;
				if (phi > TWO_PI) phi -= TWO_PI;
			}

			explicit operator glm::vec3() const {
				return glm::vec3(
					r * glm::sin(theta) * glm::sin(phi),
					r * glm::cos(theta),
					r * glm::sin(theta) * glm::cos(phi)
				);
			}
		};
	}
}