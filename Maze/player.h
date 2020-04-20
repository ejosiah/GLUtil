#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Camera.h"
#include "forward_declare.h"

struct Player {
	ncl::gl::Camera* camera;
	Cell* currentCell;

	const glm::vec3& position() const {
		return camera->getPosition();
	}

	void updatePosition(const glm::vec3& p) {
		camera->setPosition(p);
	}
};