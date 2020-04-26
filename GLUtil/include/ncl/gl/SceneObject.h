#pragma once

#include "Scene.h"
#include "gl.h"

namespace ncl {
	namespace gl {
		class SceneObject {
		public:
			SceneObject(Scene* scene = nullptr) :_scene{ scene } {
			}

			virtual void init() {

			}

			virtual void render() {

			}

			virtual void update(float t) {

			}

			virtual void processInput(const Key& key) {

			}

		protected:

			virtual Scene& scene() {
				return *_scene;
			}

			Scene* _scene;
		};
	}
}