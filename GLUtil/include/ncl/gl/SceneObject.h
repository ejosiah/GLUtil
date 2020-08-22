#pragma once

#include "Scene.h"
#include "gl.h"
#include "Shader.h"
#include "Drawable.h"

namespace ncl {
	namespace gl {
		class SceneObject : public Drawable {
		public:
			SceneObject(Scene* scene = nullptr) :_scene{ scene } {
			}

			virtual void init() {

			}

			virtual void render(bool shadowMode = false) {

			}

			virtual void update(float t) {

			}

			virtual void processInput(const Key& key) {

			}

			virtual void draw(Shader& shader) {

			}

		protected:

			virtual Scene& scene() {
				return *_scene;
			}

			Scene* _scene;
		};
	}
}