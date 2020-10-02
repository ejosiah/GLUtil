#pragma once

#include "slider.h"

namespace ncl {
	namespace ui {
		class ArcSlider : public Slider, public gl::_3DMotionEventHandler {
		public:
			ArcSlider(gl::Scene& scene, std::string label, float radius = 100.f, glm::vec2 pos = glm::vec2(0)) :Slider(scene, label, pos + glm::vec2(radius, 0)), radius(radius) {
				_width = radius * 2;
				_height = radius;
				angle = 30;
				this->limit = limit;
			}

			virtual void init() {
				createCircle();
				scene.addMouseClickListener([&](gl::Mouse& mouse) {
					if (isVisible()) {
						if (mouse.left.status == gl::Mouse::Button::PRESSED) {
							glm::vec3 p = scene.mousePositionInScene(xform, cam.projection);
							glm::vec3 c = head->mapTo<glm::vec3, glm::vec3>(0, 0, [&p](glm::vec3* points) { return points[0]; });

							glm::vec3 pc = p - c;
							if (dot(pc, pc) < radius * radius) {
								active = true;
							}
							else {
								active = false;
							}

						}
					}
				});
			};

			void createCircle() {
				float r = radius;
				float steps = 100;
				gl::Mesh m;
				float two_pi = glm::two_pi<float>();
				float step = glm::pi<float>() / steps;

				float theta = 0;
				for (int i = 0; i < steps; i++, theta += step) {
					m.positions.push_back({ r * glm::cos(theta), r * glm::sin(theta), 0 });
					m.colors.push_back(fgColor);
				}


				m.primitiveType = GL_LINE_STRIP;
				circle = new gl::ProvidedMesh(m);

				auto v0 = glm::vec3{ r * glm::cos(glm::radians(angle)), r * glm::sin(glm::radians(angle)), 0 };

				m.clear();
				m.positions.push_back(glm::vec3(0));
				m.positions.push_back(v0);
				m.colors.push_back(fgColor);
				m.colors.push_back(fgColor);
				m.primitiveType = GL_LINES;
				handle = new gl::ProvidedMesh(m);
				fixedHandle = new gl::ProvidedMesh(m);

				m.clear();
				m.positions.push_back(v0);
				m.colors.push_back(fgColor);

				step = two_pi / steps;
				theta = 0;
				glm::vec3 c = v0;
				r *= 0.05;
				for (int i = 0; i < steps; i++, theta += step) {
					m.positions.push_back(c + glm::vec3{ r * glm::cos(theta), r * glm::sin(theta), 0 });
					m.colors.push_back(fgColor);
				}
				m.primitiveType = GL_TRIANGLE_FAN;
				head = new gl::ProvidedMesh(m);
				radius = r;
			}

			virtual void onMotion(const gl::_3DMotionEvent& evt) override {
				if (active && isVisible()) {
					if (evt.rotation.y > 0) {
						angle += 1.6;
					}
					else if (evt.rotation.y < 0) {
						angle -= 1.6;
					}
					angle = glm::clamp(angle, 0.0f, limit);

					float theta = glm::radians(angle);

					float r = 100;
					glm::vec3 c = { r * glm::cos(theta), r * glm::sin(theta), 0 };
					handle->update2<glm::vec3>(gl::VertexAttrib::Position, [&](glm::vec3* points) {
						points[1] = c;
					});
					head->update2<glm::vec3>(gl::VertexAttrib::Position, [&](glm::vec3* points) {
						theta = 0;
						points[0] = c;
						points++;
						r *= 0.05;
						float two_pi = glm::two_pi<float>();
						float step = two_pi / steps;
						for (int i = 0; i < steps; i++, theta += step) {
							*points = c + glm::vec3{ r * glm::cos(theta), r * glm::sin(theta), 0 };
							points++;
						}
					});
				}
			};

			virtual void onNoMotion() override {}


			virtual void render(gl::Shader& s) override {
				cam.model = xform;
				send(cam);
				shade(circle);
				shade(handle);
				shade(fixedHandle);
				shade(head);
			};

			virtual void renderFont() override {
				ss << _label << ": ";
				ss << std::setprecision(3) << angle;
				auto offset = -font->size;
				glm::vec2 pos = glm::vec2(xform * glm::vec4{ offset, offset, 0, 1 });
				font->render(ss.str(), pos.x, scene.height() - pos.y);
				ss.str("");
				ss.clear();
			}

			virtual void update(float dt) {}

			virtual float value() {
				return angle;
			};

		public:
			gl::ProvidedMesh * circle;
			gl::ProvidedMesh * handle;
			gl::ProvidedMesh * fixedHandle;
			gl::ProvidedMesh* head;
			float radius;
			float angle = 0;
			float limit;
			bool active = false;
			int steps = 100;
		};
	}
}