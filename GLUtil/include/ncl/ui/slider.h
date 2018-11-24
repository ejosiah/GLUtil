#pragma once

#include "component.h"

namespace ncl {
	namespace ui {
		class Slider : public Component {
		public:
			Slider(gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0)) :Component(scene, label, pos) {
			}

			~Slider() = default;

			virtual float value() = 0;

			virtual void value(float v) {};
		};
	}
}