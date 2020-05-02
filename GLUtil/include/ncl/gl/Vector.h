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
			Vector(glm::vec3 value, glm::vec3 origin = glm::vec3(0), float s = 1.0, glm::vec4 color = randomColor(), bool fixedLength = false) {
				using namespace glm;

				this->origin = origin;
				this->fixedLength = fixedLength;
				this->scale = s;
				this->value = value;
				
				vec3 v1(0, 0, -1);	// primitives face towards -z, so we have to rotate (0,0,-1) to v2
				vec3 v2 = normalize(value);
				vec3 axis = normalize(cross(v1, v2));
				if(abs(v1) == abs(v2)){
					axis = vec3(1, 0, 0);
				}

				float angle = degrees(acos(dot(v1, v2)));

				auto rotate = mat4_cast(fromAxisAngle(axis, angle));
				float l = fixedLength? 1 : length(value);

				mat4 headTransform = glm::scale(mat4(1), { s, s, s }) *  translate(mat4(1), origin) * rotate * translate(mat4(1), { 0, 0, -l });
				mat4 bodyTransform = glm::scale(mat4(1), { s, s, s }) * translate(mat4(1), origin) * rotate * mat4(1);
				body = new Cylinder(0.01, l - 0.2, 10, 10, color, 1, bodyTransform);
				head = new Cone(0.055, 0.2, 10, 10, color, 1, headTransform);
				
			}

			bool orient(glm::vec3 oldVec, glm::vec3 newVec, glm::vec3 origin, glm::mat4& headTransform, glm::mat4& bodyTransform) {
				using namespace glm;
				float s = scale;
				vec3 v1 = normalize(oldVec);	// primitives face towards -z, so we have to rotate (0,0,-1) to v2
				vec3 v2 = normalize(newVec);
				float angle = degrees(acos(dot(v1, v2)));

				if (abs(v1) == abs(v2) && angle != std::numeric_limits<float>::infinity() && angle < 1.0f) {
					headTransform = bodyTransform = mat4{ 1 };
					return false;
				}

				vec3 axis = normalize(cross(v1, v2));
				

				auto rotate = mat4_cast(fromAxisAngle(axis, angle));
				float l = fixedLength ? 1 : length(value);

				headTransform =   rotate;
				bodyTransform = rotate;
				return true;
			}

			virtual ~Vector() {
				delete body;
				delete head;
			}
		
			virtual void draw(Shader& shader) override {
				head->draw(shader);
				body->draw(shader);
			};

			void move(glm::vec3 pos) {
				auto delta = pos - origin;
				origin = pos;
				glm::mat4 model = translate(glm::mat4(1), delta);
				body->update2<glm::vec3>(0, [&](glm::vec3* ptr) {
					glm::vec4 p = glm::vec4{ *ptr, 1 };
					*ptr = glm::vec3(model * p);
				});
				head->update2<glm::vec3>(0, [&](glm::vec3* ptr) {
					glm::vec4 p = glm::vec4{ *ptr, 1 };
					*ptr = glm::vec3(model * p);
				});
			}

			void update(glm::vec3 newValue) {
				glm::mat4 headTransform;
				glm::mat4 bodyTransform;
				auto doUpdate = orient(value, newValue, origin, headTransform, bodyTransform);

				if (doUpdate) {
					body->update2<glm::vec3>(0, [&](glm::vec3* ptr) {
						glm::vec4 p = glm::vec4{ *ptr, 1 };
						*ptr = glm::vec3(bodyTransform * p);
						});
					head->update2<glm::vec3>(0, [&](glm::vec3* ptr) {
						glm::vec4 p = glm::vec4{ *ptr, 1 };
						*ptr = glm::vec3(headTransform * p);
						});
					value = newValue;
				}
			}

		private:
			Cylinder* body;
			Cone* head;
			glm::vec3 origin;
			glm::vec3 value;
			bool fixedLength;
			float scale;
		};
	}
}