#pragma once

#include "../gl/util.h"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <tuple>

namespace ncl {
	namespace sampling {

		static std::function<real(void)> crn = rngReal(0, 1);

		inline float radicalInverse_VdC(unsigned int bits) {
			bits = (bits << 16u) | (bits >> 16u);
			bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
			bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
			bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
			bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
			return float(bits) * 2.3283064365386963e-10; // / 0x100000000
		}

		std::tuple<float, float> jittered(glm::vec2 u, unsigned int N) {
			float _1_over_N = 1 / float(N);

			return std::make_pair((u.x + crn()) * _1_over_N, (u.y + crn()) * _1_over_N);
		}

		std::tuple<float, float> hammersley2d(unsigned int i, unsigned int N) {
			return std::make_tuple(float(i) / float(N), radicalInverse_VdC(i));
		}

		glm::vec2 hammersley2dPoint(unsigned int i, unsigned int N) {
			return glm::vec2{ float(i) / float(N), radicalInverse_VdC(i) };
		}


		inline glm::vec3 pointInSphere(float x, float y) {
			float sin0 = ::sqrt(1 - x * x);
			float phi = 2 * glm::pi<float>() * y;
			return glm::vec3{
				::cos(phi) * sin0,
				::sin(phi) * sin0,
				::cos(x)
			};
		}


		inline std::vector<glm::vec3> hammersleySphere(int numPoints) {
			std::vector<glm::vec3> points;
			points.resize(numPoints);

			int i = -1;
			std::generate(points.begin(), points.end(), [=] () mutable {
				auto [x, y] = hammersley2d(++i, numPoints);
				return pointInSphere(x, y);
			});
			return points;
		}

		inline std::vector<glm::vec3> randomSphere(int numPoints) {
			std::vector<glm::vec3> points;
			points.resize(numPoints);

			std::generate(points.begin(), points.end(), [=]() mutable {
				return pointInSphere(crn(), crn());
			});
			return points;
		}

		inline std::vector<glm::vec3> jitteredSphere(int numPoints) {
			std::vector<glm::vec3> points;
			int n = ::sqrt(numPoints);
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					auto [x, y] = jittered({ j, i }, n);
					points.push_back(pointInSphere(x, y));
				}
			}
			return points;
		}

		inline std::vector<glm::vec3> fibonacciSpiralSphere(int numPoints) {
			std::vector<glm::vec3> points(numPoints);

			const float gr = (::sqrt(5.0f) + 1.0) / 2.0f;
			const float ga = (2.0f - gr) * glm::two_pi<float>();

			int i = 0;
			std::generate(points.begin(), points.end(), [&]() {
				const float lat = asin(-1.0 + 2.0 * float(i) / (numPoints + 1));
				const float lon = ga * i;
				i++;
				return glm::vec3{
					::cos(lon)* ::cos(lat),
					::sin(lon)* ::cos(lat),
					::cos(lat)
				};
			});

			return points;
		}

	}
}