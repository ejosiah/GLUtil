#pragma once

#include <glm/glm.hpp>
#include "../gl/StorageBufferObj.h"
#include "../gl/common.h"
#include <vector>

namespace ncl {
	namespace ray_tracing {
#pragma pack(push, 1)
		struct Ray {
			glm::vec3 origin = glm::vec3(0); 
			padding_4 padding0;
			glm::vec3 direction = glm::vec3(0);
			float tMax = 0.0f;
		};
#pragma pack(pop)
	}
}