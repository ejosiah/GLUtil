#pragma once

#include <glm/glm.hpp>

namespace ncl {
	namespace geom {
		struct Triangle {
			glm::vec3 a;
			glm::vec3 b;
			glm::vec3 c;
		};

		glm::vec3 barycenter(const glm::vec3& p, const Triangle& triangle) {
			glm::vec3 v0 = triangle.b - triangle.a;
			glm::vec3 v1 = triangle.c - triangle.a;
			glm::vec3 v2 = p - triangle.a;

			float d00 = glm::dot(v0, v0);
			float d10 = glm::dot(v1, v0);
			float d11 = glm::dot(v1, v1);
			float d20 = glm::dot(v2, v0);
			float d21 = glm::dot(v2, v1);

			float A = d00 * d11 - d10 * d10;

			if (A == 0.0f) {	// use close enough test
				return glm::vec3(0);
			}

			float v = (d20 * d11 - d10 * d21) / A;
			float w = (d00 * d21 - d20 * d10) / A;
			float u = 1 - v - w;
			return { u, v, w };
		}

		glm::vec3 barycenter(const glm::vec3 p, const glm::vec3& a, const glm::vec3& b, const glm::vec3 c) {
			return barycenter(p, { a, b, c });
		}

	}
}