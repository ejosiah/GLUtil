#pragma once
#include <glm/glm.hpp>
#include <iostream>

namespace ncl {
	namespace gl {
		namespace geom {
			struct Plane {
				glm::vec3 n;
				float d;

				Plane() = default;

				Plane(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2) {
					n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
					d = dot(n, v0);
				}

				Plane& operator*=(float s) {
					n *= s;
					d *= s;
					return *this;
				}
			};

			struct Triangle {
				float tid;
				Plane p;
				Plane bc;
				Plane ca;

				Triangle() = default;

				Triangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2) {
					p = Plane(v0, v1, v2);
					bc.n = glm::cross(p.n, v2 - v1);
					bc.d = glm::dot(bc.n, v1);
					ca.n = glm::cross(p.n, v0 - v2);
					ca.d = glm::dot(ca.n, v2);

					bc *= 1.0f / (glm::dot(v0, bc.n) - bc.d);
					ca *= 1.0f / (glm::dot(v1, ca.n) - ca.d);
				}
			};

			std::ostream& operator<<(std::ostream& out, const Plane& p) {
				out << "Plane( n:[" << p.n.x << ", " << p.n.y << ", " << p.n.z  << "], d:" << p.d << ")";
				return out;
			}

			std::ostream& operator<<(std::ostream& out, const Triangle t) {
				out << "Triangle(" << "p: " << t.p << ", bc: " << t.bc << ", ca: " << t.ca << ")";
				return out;
			}
		}
	}
}