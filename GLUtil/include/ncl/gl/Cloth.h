#pragma once

#include "Plane.h"
#include "TransformFeedBack.h"
#include "textures.h"

namespace ncl {
	namespace gl {
		class Cloth : public Drawable {
		public:
			Cloth() {}


		virtual void draw(Shader& shader) override {
		}

		void update(float dt) {

		}
			
		private:

		};
	}
}