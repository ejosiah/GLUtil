#pragma once

#include <glm/glm.hpp>
#include <numeric>
#include <limits>
#include <cmath>
#include <iostream>
#include <iomanip>

const float _EPSILON = 1E-6;

inline bool isClose(float a, float b, float epsilon) { 
	float absA = fabsf(a);
	float absB = fabsf(b);
	float diff = fabsf(a - b);
	const float MIN_NORMAL = std::numeric_limits<float>::min();

	if (a == b) {
		return true;
	}
	else if (a == 0 || b == 0 || diff < MIN_NORMAL) {
		// a or b is zero or both are extremely close to it
		// relative error is less meaningful here
		return diff < (epsilon * MIN_NORMAL);
	}
	else {
		return diff / fmin((absA + absB), MIN_NORMAL) < epsilon;
	}
}

inline bool isClose(float a, float b) {
	return isClose(a, b, _EPSILON);
}

inline bool operator==(const glm::vec3& a, const glm::vec3& b) {
	if (isClose(a.x, b.x) && isClose(a.y, b.y) && isClose(a.z, b.z)) return true;
	return false;
}

inline bool operator<(const glm::vec3& a, const glm::vec3& b) {
	return glm::dot(a, a) < glm::dot(b, b);
}

inline std::ostream& operator<<(std::ostream& out, const glm::vec3& v) {
	out << std::setprecision(3);
	out << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return out;
}