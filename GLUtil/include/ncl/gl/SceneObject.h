#pragma once

#include "gl.h"

namespace ncl {
	namespace gl {
		class SceneObject {
		public:
			SceneObject(const Scene& scene) :scene{ scene } {
			}

			void render() {

			}

			void update(float t) {

			}

		protected:
			const Scene& scene;
		};
	}
}