#pragma once

#include "Cylinder.h"
#include "Cone.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "orientation.h"

namespace ncl {
	namespace gl {
		class Vector : public Drawable {
		public: 
			Vector(glm::vec3 value, glm::vec3 origin = glm::vec3(0), float s = 1.0, glm::vec4 color = randomColor()) {
				using namespace glm;
				
				vec3 v1(0, 0, -1);
				vec3 v2 = normalize(value);
				vec3 axis = normalize(cross(v1, v2));
				if(abs(v1) == abs(v2)){
					axis = vec3(1, 0, 0);
				}

				float angle = degrees(acos(dot(v1, v2)));

				auto rotate = mat4_cast(fromAxisAngle(axis, angle));
				float l = length(value);

				mat4 headTransform = scale(mat4(1), { s, s, s }) *  translate(mat4(1), origin) * rotate * translate(mat4(1), { 0, 0, -l });
				mat4 bodyTransform = scale(mat4(1), { s, s, s }) * translate(mat4(1), origin) * rotate * mat4(1);
				body = new Cylinder(0.01, l - 0.2, 10, 10, color, 1, bodyTransform);
				head = new Cone(0.055, 0.2, 10, 10, color, 1, headTransform);
			}
		
			virtual void draw(Shader& shader) override {
				head->draw(shader);
				body->draw(shader);
			};

		private:
			Cylinder* body;
			Cone* head;
		};
	}
}