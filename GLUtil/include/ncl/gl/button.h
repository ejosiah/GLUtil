#pragma once

#include <functional>
#include <vector>
#include "UICore.h"
#include "events.h"
#include "Font.h"
#include <cstring>

namespace ncl {
	namespace gl {
		namespace ui {

			using ActionListener = std::function<void(const ActionEvent*)>;

			class Button : public RelativeComponent {
			public:
				enum State { PRESSED, RELEASED };

				Button(const char* text) :text(text) {
					font = Font::Arial(10, Font::BOLD, BLACK);
				//	std::string str = "button[" + std::string(text) + "]";
				//	strcpy_s(const_cast<char*>(name), str.size(), str.c_str());
					name = "button[]";
					auto fontSize = font->sizeOf(text);
					setDimensions(fontSize.x + 30, 20);
					canvas = UI::templateCanvas;
					setPosition(0, 0);
				}

				Button(Icon* icon) {
					padding.right = padding.left = padding.top = padding.bottom = 5;
				}
				virtual ~Button() {
					delete font;
				};

				void addActionListener(ActionListener l) {
					actionListeners.push_back(l);
				}

				void render() {
					renderText();
					renderBodabsY();
					renderHighLight();
					renderShadow();
				}

				void renderBodabsY() {
					glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);

					transformation = glm::translate(pTransform, { absX(), absY(), 0 });
					transformation = glm::scale(transformation, { width(), height(), 1 });

					auto color = glm::vec4(0);

					glm::mat4 mvp = UI::projection * transformation;
					shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
					shader.sendUniform4fv("color", 1, glm::value_ptr(theme.background));
					canvas->draw(shader);
				}


				void renderHighLight() {
					glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);

					transformation = glm::translate(pTransform, { absX() - 1, absY() + 1, 0 });
					transformation = glm::scale(transformation, { width(), height(), 1 });

					glm::mat4 mvp = UI::projection * transformation;
					shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));

					if (state == RELEASED) {
						shader.sendUniform4fv("color", 1, glm::value_ptr(theme.bevelTop));
					}
					else {
						shader.sendUniform4fv("color", 1, glm::value_ptr(theme.bevelBottom));
					}
					
					canvas->draw(shader);
				}

				void renderShadow() {
					glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);

					transformation = glm::translate(pTransform, { absX() + 1, absY() - 1, 0 });
					transformation = glm::scale(transformation, { width(), height(), 1 });

					glm::mat4 mvp = UI::projection * transformation;
					shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
					if (state == RELEASED) {
						shader.sendUniform4fv("color", 1, glm::value_ptr(theme.bevelBottom));
					}
					else {
						shader.sendUniform4fv("color", 1, glm::value_ptr(theme.bevelTop));
					}
					canvas->draw(shader);
				}

				void renderText() {
					glm::mat4 model = parent ? parent->getTransformation() : glm::mat4(1);
					model = glm::translate(model, { absX(), absY(), 0 });

					glm::ivec2 pos = glm::ivec2(model * glm::vec4(0, 5, 0, 1));

					font->render(text, pos.x + 5, pos.y);
				}

				void fireActionEvent() {
					const ActionEvent* event = new ActionEvent(this, "pressed");
					for (ActionListener l : actionListeners) {
						l(event);
					}
					delete event;
				}

				virtual void handleMouseClick(Mouse& mouse) override {
					if (mouse.left.status == Mouse::Button::PRESSED) {
						state = PRESSED;
					}
					else if (mouse.left.status == Mouse::Button::RELEASED) {
						state = RELEASED;
					}
				}

				virtual void handleMouseMove(Mouse& mouse) {}
				virtual void handleKeyPress(const Key&) {}

			private:
				const char* text;
				Font* font;
				Icon* icon;
				std::vector<ActionListener> actionListeners;
				State state = RELEASED;
				struct {
					float right, top, bottom, left;
				} padding;
			};
		}
	}
}