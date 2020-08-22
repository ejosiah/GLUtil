#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include "util.h"
#include <limits>
#include <cmath>
#include <vector>
#include <fstream>

#ifndef CHECK_GL_ERRORS
#define CHECK_GL_ERRORS  assert(glGetError()==GL_NO_ERROR);
#endif

#ifndef TEXTURE(id)
#define TEXTURE(id) GL_TEXTURE0 + id
#endif // !TEXTURE(id)

constexpr int paddingId = 0;


namespace ncl {

	struct padding_1 {
		char p0;
	};
	struct padding_2 {
		char p0, p1;
	};

	struct padding_3 {
		char p0, p1, p2;
	};

	struct padding_4 {
		int p0;
	};

	struct padding_8 {
		int p0, p1;
	};

	struct padding_12 {
		int p0, p1, p2;
	};

	struct padding_16 {
		int p0, p1, p2, p3;
	};

	namespace gl {
#define BUFFER_OFFSET(offset) ((void*)offset)

		const float EPSILON = 0.000001;

		const bool DONT_CULL_BACK_FACE = false;

		const std::function<void()> VOID_FUNC = [] {};

		bool closeEnough(float x, float y) { return abs(x - y) <= EPSILON * (abs(x) + abs(y) + 1.0f); }

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
		const color YELLOW = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		const color CYAN = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		const color MAGENTA = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

		glm::vec4 iColor(int r, int g, int b) {
			return { float(r)/255, float(g)/255, float(b)/255, 0};
		}

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

		class Chain3DMotionEventHandler : public _3DMotionEventHandler {
		public:
			Chain3DMotionEventHandler(std::vector<_3DMotionEventHandler*> handlers):handlers(handlers) {

			}

			virtual ~Chain3DMotionEventHandler() {
				for (auto handler : handlers) delete handler;
			}

			virtual void onMotion(const _3DMotionEvent& event) override {
				for (auto handler : handlers) {
					handler->onMotion(event);
				}
			};
			virtual void onNoMotion() override {
				for (auto handler : handlers) {
					handler->onNoMotion();
				}
			};
		private:
			std::vector<_3DMotionEventHandler*> handlers;
		};

		struct buf_vec4 {
			//union {
			//	struct { float x, y, z, w; };
			//	float data[4];
			//};
			float data[4];

			buf_vec4() = default;

			buf_vec4(const glm::vec4& v) {
				data[0] = v.x;
				data[1] = v.y;
				data[2] = v.z;
				data[3] = v.w;
			}

			operator glm::vec4() const {
				return glm::vec4(data[0], data[1], data[2], data[3]);
			}
		};
		bool operator==(const buf_vec4& a, const buf_vec4& b) {
			return a.data[0] == b.data[0]
				&& a.data[1] == b.data[1]
				&& a.data[2] == b.data[2]
				&& a.data[3] == b.data[3];
		}

		struct buf_vec4_hash {

			size_t operator()(buf_vec4 v) const {
				size_t seed = 3;
				for (int i = 0; i < 3; i++) {
					int v_comp = v.data[i];
					seed ^= v_comp + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				}
				return seed;
			}
		};

		using Exec = std::function<void()>;
	}
}