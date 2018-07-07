#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {
		struct Triangle;

		glm::vec3 barycenter(const glm::vec3& p, const Triangle& triangle);

		glm::vec3 barycenter2(const glm::vec3& p, const Triangle& t);

		struct Triangle {
			glm::vec3 a;
			glm::vec3 b;
			glm::vec3 c;

			bool contains(const glm::vec3& point) const {
				glm::vec3 uvw = barycenter(point, *this);
				return uvw.y >= 0 && uvw.z >= 0 && (uvw.y + uvw.z) <= 1;
			}
		};

		glm::vec3 barycenter(const glm::vec3& p, const Triangle& triangle) {
		//	return barycenter2(p, triangle);
			glm::vec3 v0 = triangle.b - triangle.a;
			glm::vec3 v1 = triangle.c - triangle.a;
			glm::vec3 v2 = p - triangle.a;

			float d00 = glm::dot(v0, v0);
			float d10 = glm::dot(v1, v0);
			float d11 = glm::dot(v1, v1);
			float d20 = glm::dot(v2, v0);
			float d21 = glm::dot(v2, v1);

			float a = d00 * d11 - d10 * d10;

			if (a == 0.0f) {	// use close enough test
				return glm::vec3(0);
			}

			float v = (d20 * d11 - d10 * d21) / a;
			float w = (d00 * d21 - d20 * d10) / a;
			float u = 1 - v - w;
			return { u, v, w };
		}

		glm::vec3 barycenter2(const glm::vec3& p, const Triangle& t){
			using namespace glm;
			auto n = cross(t.b - t.a, t.c - t.a);

			auto PBC = dot(cross(t.b - p, t.c - p), normalize(n));
			auto PCA = dot(cross(t.c - p, t.a - p), normalize(n));
			auto PAB = dot(cross(t.a - p, t.b - p), normalize(n));
			auto ABC = dot(cross(t.b - t.a, t.c - t.a), normalize(n));

			float u = PBC / ABC;
		//	float v = PCA / ABC;
			
			float w = PAB / ABC;
			float v = 1 - u - w;
			return { u, v, w };
		}
	}
}