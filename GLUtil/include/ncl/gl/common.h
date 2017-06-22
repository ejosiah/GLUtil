#pragma once

#include <glm/vec4.hpp>
#include <functional>
#include <string>
#include "util.h"

namespace ncl {
	namespace gl {
#define BUFFER_OFFSET(offset) ((void*)offset)

		void clear(glm::vec3& v) {
			v.x = v.y = v.z = 0;
		}

		const std::string username = getEnv("username");

		using color = glm::vec4;
		const ncl::Random RNG;

		const color RED = glm::vec4(1, 0, 0, 1);
		const color GREEN = glm::vec4(0, 1, 0, 1);
		const color BLUE = glm::vec4(0, 0, 1, 1);
		const color WHITE = glm::vec4(1);
		const color BLACK = glm::vec4(0, 0, 0, 1);
		const color GRAY = glm::vec4(0.5f, 0.5f, 0.5f, 1);

		glm::vec4 randomColor() {
			return glm::vec4(RNG.vector(glm::vec3(0), glm::vec3(1)), 1);
		}

		void withVertexArray(const unsigned int& vaoId, std::function<void(void)> body) {
			glBindVertexArray(vaoId);
			body();
			glBindVertexArray(0);
		}

		struct _3DMotionEvent {
			glm::vec3 translation;
			glm::vec3 rotation;
		};

		class _3DMotionEventHandler {
		public:
			virtual void onMotion(const _3DMotionEvent&) = 0;
			virtual void onNoMotion() = 0;
		};

		class Void3DMotionEventHandler : public _3DMotionEventHandler {
		public:
			virtual void onMotion(const _3DMotionEvent&) override {};
			virtual void onNoMotion() override {};
		};

	}
}