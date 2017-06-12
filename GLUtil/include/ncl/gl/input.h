#pragma once

#include <glm/vec2.hpp>
#include <functional>
#include <vector>
#include "Keyboard.h"

namespace ncl {
	namespace gl {

		class Mouse {
		public:
			struct Button {
				enum Status { PRESSED, RELEASED };
				Status status = RELEASED;
			};
			Button left;
			Button middle;
			Button right;
			glm::vec2 pos;
			glm::vec2 relativePos;
			float wheelPos;
			bool _recenter = false;

			void recenter() {
				_recenter = true;
			}

			static void init() {
				instance = new Mouse();
			}

			static Mouse& get() {
				return *instance;
			}

			static void dispose() {
				delete instance;
			}

		private:
			static Mouse* instance;
		};

		Mouse* Mouse::instance;

		using KeyListener = std::function<void(const Key&)>;
		using MouseClickListner = std::function<void(Mouse&)>;
		using MouseMoveListner = std::function<void(Mouse&)>;

		std::vector<KeyListener> keyListeners;
		std::vector<MouseClickListner> mouseClickListners;
		std::vector<MouseMoveListner> mouseMoveListners;
	}
}