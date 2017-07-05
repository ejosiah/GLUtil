#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

using Orientation = glm::quat;

Orientation fromAxisAngle(const glm::vec3& axis, const float angle) {
	float w = cos(glm::radians(angle) / 2);
	glm::vec3 xyz = axis * sin(glm::radians(angle) / 2);
	return Orientation(w, xyz);
}
