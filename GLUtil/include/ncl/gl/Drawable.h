#pragma once

#include <glm/mat4x4.hpp>
#include "Shader.h"

namespace ncl {
	namespace gl {
		class Drawable {
		public:
			virtual void draw(Shader& shader) = 0;
		};
	}
}