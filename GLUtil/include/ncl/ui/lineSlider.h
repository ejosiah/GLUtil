#pragma once

#include <utility>
#include "slider.h"

namespace ncl {
	namespace ui {
		class LineSlider : public Slider {
		public:
			LineSlider(gl::Scene& scene, int length, std::string label = "", float value = 0, float lower = 0, float upper = 1, float step = 0.1, glm::vec2 pos = glm::vec2(0))
				:Slider(scene, label, pos), _length(length), _value(value), _range(std::make_tuple(lower, upper)), _step(step) {
				oldMousePos = glm::vec2(0);
				_initialValue = _value;
			}

			void init() {
				gl::Mesh m;
				m.positions.push_back({ 0, 0, 0 });
				m.positions.push_back({ _length, 0, 0 });
				m.colors = std::vector<glm::vec4>{ 2, fgColor };
				m.primitiveType = GL_LINES;

				body = new gl::ProvidedMesh(m);

				m.clear();

				glm::vec3 c = glm::vec3(0);

				if (_value != std::get<0>(_range)) {
					c.x = ((_value - std::get<0>(_range)) / (std::get<1>(_range) - std::get<0>(_range))) * _length;
				}

				float step = glm::two_pi<float>() / 100;
				float theta = 0;
				float r = 5;
				m.positions.push_back(c);
				m.colors.push_back(fgColor);
				for (int i = 0; i < 100; i++, theta += step) {
					m.positions.push_back(c + glm::vec3{ r * cos(theta), r * sin(theta), 0 });
					m.colors.push_back(fgColor);
				}
				m.primitiveType = GL_TRIANGLE_FAN;
				head = new gl::ProvidedMesh(m);
				headPos = c;

				scene.addMouseClickListener([&](gl::Mouse& mouse) {
					if (isVisible()) {
						if (mouse.left.status == gl::Mouse::Button::PRESSED && !active) {
							glm::vec3 p = scene.mousePositionInScene(xform, cam.projection);
							glm::vec3 c = head->mapTo<glm::vec3, glm::vec3>(0, 0, [&p](glm::vec3* points) { return points[0]; });

							glm::vec3 pc = p - c;
							if (dot(pc, pc) < 25) {
								active = true;
							}
						}
						else if (mouse.left.status == gl::Mouse::Button::RELEASED && active) {
							active = false;
						}
					}
				});

				scene.addMouseMouseListener([&](gl::Mouse& mouse) {
					if (active & isVisible()) {
						glm::vec2 dt = mouse.pos - oldMousePos;
						oldMousePos = mouse.pos;
						if (dt.y == 0) headPos.x += dt.x;

						update();
						_initialValue = _value;
					}
				});
			}

			void update() {
				headPos.x = glm::clamp(headPos.x, 0.0f, float(_length));
				head->update2<glm::vec3>(gl::VAOObject::Position, [&](glm::vec3* points) {
					float theta = 0;
					points[0] = headPos;
					points++;
					float r = 5;
					float two_pi = 2 * glm::pi<float>();
					float step = two_pi / 100;
					for (int i = 0; i < 100; i++, theta += step) {
						*points = headPos + glm::vec3{ r * cos(theta), r * sin(theta), 0 };
						points++;
					}
				});

				float t = length(headPos) / _length;
				_value = glm::mix(std::get<0>(_range), std::get<1>(_range), t);
			}

			virtual void render(gl::Shader& s) override {
				cam.model = xform;
				send(cam);
				shade(body);
				shade(head);
			};

			virtual void renderFont() override {
				float w = font->size * _label.size();
				float h = font->size;
				ss << _label << ": ";
				ss << std::setprecision(2) << _value;
				glm::vec2 pos = glm::vec2(xform * glm::vec4{ (_length - w) / 2, -h, 0, 1 });
				font->render(ss.str(), pos.x, scene.height() - pos.y);
				ss.str("");
				ss.clear();
			}

			virtual float value() override {
				return _value;
			}


			void move(float delta) {
				float v = _initialValue * delta;
				if (v >= std::get<0>(_range) && v <= std::get<1>(_range)) {
					headPos.x = ((v - std::get<0>(_range)) / (std::get<1>(_range) - std::get<0>(_range))) * _length;
					update();
				}
			};

			bool active;

		private:
			gl::ProvidedMesh * body;
			gl::ProvidedMesh* head;
			int _length;
			float _value;
			float _initialValue;
			std::tuple<float, float> _range;
			float _step;
			glm::vec3 headPos;
			glm::vec2 oldMousePos;

		};
	}
}