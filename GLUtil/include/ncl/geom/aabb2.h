#pragma once


#include <glm/glm.hpp>
#include <vector>
#include <limits>

namespace ncl {
	namespace geom {
		namespace bvol {
#pragma pack (push, 1)
			struct AABB2 {
				glm::vec4 min = glm::vec4{ std::numeric_limits<float>::max() };
				glm::vec4 max = glm::vec4{ std::numeric_limits<float>::lowest() };
			};
#pragma pack (pop)

			namespace aabb {

				inline AABB2 create(glm::vec3* points, int n) {
					AABB2 aabb;
					aabb.min = aabb.max = glm::vec4(*points, 0);
					for (int i = 1; i < n; i++) {
						aabb.min = glm::min(aabb.min, glm::vec4(*(points + i), 0));
						aabb.max = glm::max(aabb.max, glm::vec4(*(points + i), 0));
					}
					return aabb;
				}

				inline AABB2 create(std::vector<glm::vec3> points) {
					return create(&points[0], points.size());
				}

				inline glm::vec3 diagonal(const AABB2& aabb) {
					return aabb.max.xyz - aabb.min.xyz;
				}

				inline glm::vec3 halfWidth(const AABB2& aabb) {
					return diagonal(aabb) * 0.5f;
				}

				inline float sufraceArea(const AABB2& aabb) {
					auto d = diagonal(aabb);
					return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
				}

				inline float volume(const AABB2& aabb) {
					auto d = diagonal(aabb);
					return d.x * d.y * d.z;
				}

				inline int maxExtent(const AABB2& aabb) {
					auto d = diagonal(aabb);
					if (d.x > d.y && d.x > d.z)
						return 0;
					else if (d.y > d.z)
						return 1;
					else
						return 2;
				}

				inline AABB2 Union(const AABB2& a, const AABB2& b) {
					AABB2 aabb;
					aabb.min = glm::min(a.min, b.min);
					aabb.max = glm::max(a.max, b.max);
					return aabb;
				}

				inline AABB2 Union(const AABB2& a, const glm::vec3& p) {
					AABB2 aabb;
					aabb.min = glm::min(a.min, glm::vec4(p, 0));
					aabb.max = glm::max(a.max, glm::vec4(p, 0));
					return aabb;
				}

				inline AABB2 Union(const AABB2& bounds, const std::vector<glm::vec3> points) {
					AABB2 aabb = bounds;
					for (auto p : points) {
						aabb = Union(aabb, p);
					}
					return aabb;
				}

				inline glm::vec3 center(const AABB2& aabb) {
					return (aabb.min.xyz + aabb.max.xyz) * 0.5f;
				}

				inline glm::vec3 offset(const AABB2& aabb, const glm::vec3& p) {
					glm::vec3 o = p - aabb.min.xyz;
					if (aabb.max.x > aabb.min.x) o.x /= aabb.max.x - aabb.min.x;
					if (aabb.max.y > aabb.min.y) o.y /= aabb.max.y - aabb.min.y;
					if (aabb.max.z > aabb.min.z) o.z /= aabb.max.z - aabb.min.z;
					return o;
				}

				inline bool encloses(const AABB2& aabb, const glm::vec3 p) {
					using namespace glm;
					auto aMin = vec3(aabb.min);
					auto aMax = vec3(aabb.max);
					auto signa = sign(p - aMin);
					auto signb = sign(aMax - p);
					auto res = dot(signa, signb);
					return res == 3;
				}

				inline glm::vec3 closestPoint(const AABB2& aabb, const glm::vec3 p) {
					auto q = p;
					q = glm::max(q, glm::vec3(aabb.min));
					q = glm::min(q, glm::vec3(aabb.max.xyz));

					return q;
				}
			}
		}
	}
}