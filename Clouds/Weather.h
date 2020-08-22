#pragma once

#include <glm/glm.hpp>

struct Weather {
	float cloud_coverage = 1.0f;
	float cloud_type = 0.7f;
	float percipitation = 0.0f;
	glm::vec3 wind_direciton = { 1.0, 0.0, 0.0 };
	float cloud_speed = 10.0;
};