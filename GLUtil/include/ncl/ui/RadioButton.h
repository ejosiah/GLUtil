#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "component.h"
#include "panel.h"
#include "../gl/Scene.h"
#include <cmath>
#include <algorithm>

namespace ncl {
	namespace ui {
		class RadioButton : public Component {
		public:
			RadioButton(gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0), int w = 0, int h = 0) :
				Component(scene, label, pos, w, h) {
				auto fSize = font->sizeOf(label);
				_width = fSize.x + 30;
				_height = font->maxHeight + 6;
			}

			friend class RadioGroup;

			virtual void init() override {
				gl::Mesh m;
				float step = glm::two_pi<float>() / 100;
				float theta = 0;
				float r = 5;
				m.colors.push_back(fgColor);
				for (int i = 0; i < 100; i++, theta += step) {
					m.positions.push_back(glm::vec3{ r * cos(theta), r * sin(theta), 0 });
					m.colors.push_back(fgColor);
				}
				m.primitiveType = GL_LINE_LOOP;
				outer = new gl::ProvidedMesh(m);

				m.clear();

				theta = 0;
				r = 3;
				m.positions.push_back(glm::vec3(0));
				m.colors.push_back(fgColor);
				for (int i = 0; i < 100; i++, theta += step) {
					m.positions.push_back(glm::vec3{ r * cos(theta), r * sin(theta), 0 });
					m.colors.push_back(fgColor);
				}
				m.primitiveType = GL_TRIANGLE_FAN;
				inner = new gl::ProvidedMesh(m);

				m.clear();
				initialized = true;
			};

			virtual void render(gl::Shader& s) override {
				cam.model = xform;
				send(cam);
				if (_selected) {
					shade(inner);
				}
				shade(outer);
			};

			void select() {
				_selected = true;
			}

			void deselect() {
				_selected = false;
			}

			bool selected() {
				return _selected;
			}

			void fire() {
				for (auto handle : handlers) handle();
			}

			void onSelect(std::function<void()> handler) {
				handlers.push_back(handler);
			}

			virtual void update(float dt) {}

			virtual void renderFont() {
				auto fSize = font->sizeOf(_label);
				auto pos = glm::vec2(xform * glm::vec4{ 10, -3, 0, 1 });
				font->render(_label, pos.x, scene.height() - pos.y);
			}

		protected:
			bool _selected = false;
			gl::ProvidedMesh* outer;
			gl::ProvidedMesh* inner;
			std::vector<std::function<void()>> handlers;

		};

		class RadioGroup : public Panel {
		public:
			RadioGroup(int w, int h, gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0)) 
				:Panel(w, h, scene, label, pos) {

			}

			void addRadioButton(RadioButton* button) {
				_height = fmax(_height, button->height());

				if (!button->initialized) button->init();
				buttons.push_back(button);

				float xOffset = _width / buttons.size();
				float yOffset = _height / 2;
				for (int i = 0; i < buttons.size(); i++) {
					auto b = buttons[i];
					b->pos({ 20 + i * xOffset, yOffset });
				}

				addChild(button);
			}

			void init() override {
				Panel::init();
				scene.addMouseClickListener([&](gl::Mouse& mouse) {
					if (isVisible()) {
						for (auto button : buttons) {
							if (mouse.left.status == gl::Mouse::Button::PRESSED) {
								glm::vec3 p = scene.mousePositionInScene(button->xform, button->cam.projection);
								glm::vec3 c = button->inner->mapTo<glm::vec3, glm::vec3>(0, 0, [&p](glm::vec3* points) { return points[0]; });

								glm::vec3 pc = p - c;
								if (dot(pc, pc) < 25) {
									if (!button->_selected) {
										auto itr = std::find_if(buttons.begin(), buttons.end(), [&](RadioButton* b) { return b->_selected; });
										if (itr != buttons.end()) (*itr)->deselect();
										button->select();
										button->fire();
									}
								}
							}
						}
					}
				});
			}

		protected:
			std::vector<RadioButton*> buttons;
		};

	}
}