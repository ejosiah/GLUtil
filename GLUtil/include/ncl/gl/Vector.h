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
			Vector(glm::vec3 value, glm::vec3 origin = glm::vec3(0), float scale = 1.0, glm::vec4 color = randomColor()) {
				
				glm::vec3 v1(0, 0, -1);
				glm::vec3 v2 = normalize(value);
				glm::vec3 axis = normalize(cross(v1, v2));
				if(glm::abs(v1) == glm::abs(v2)){
					axis = glm::vec3(1, 0, 0);
				}

				float angle = glm::degrees(glm::acos(dot(v1, v2)));

				auto rotate = glm::mat4_cast(fromAxisAngle(axis, angle));
				float l = length(value);

				glm::mat4 headTransform = glm::translate(glm::mat4(1), origin) * rotate * glm::translate(glm::mat4(1), { 0, 0, -l });
				glm::mat4 bodyTransform = glm::translate(glm::mat4(1), origin) * rotate * glm::mat4(1);
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