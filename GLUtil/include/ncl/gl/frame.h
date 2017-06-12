#pragma once
#include <functional>
#include <vector>
#include "UICore.h"
#include "events.h"
#include "Font.h"
#include <cstring>
#include <sstream>

namespace ncl {
	namespace gl {
		namespace ui {

			class Frame : public Container {
				enum State { RESTING, DRAGGING };
			public:
				Frame(const char* title = ""):title(title){
					titleBarHeight = 10;
					name = "frame";
				//	font = Font::Arial(10, Font::BOLD, BLACK);
					state = RESTING;
				}

				~Frame() {
					delete font;
				}

				virtual void render() override {
					shader.use();
					glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);
					transformation = glm::translate(pTransform, { x(), y() - titleBarHeight, 0 });
					for (Component* child : children) {
						child->render();
					}
					transformation = glm::translate(pTransform, { x(), y() + titleBarHeight, 0 });
					renderBackground();
					renderBorder();
					shader.unUse();
				}

				virtual void handleMouseClick(Mouse& mouse) override {
					if (state == DRAGGING || isInDragZone(mouse)) {
						updateState(mouse);
					}
					else {
						Container::handleMouseClick(mouse);
					}

				}

				virtual void handleMouseMove(Mouse& mouse) override {
					if (state == DRAGGING) {
						stringstream ss;
						ss << "[" << mouse.pos.x << ", " << mouse.pos.y << "]";
						Logger::get("Frame").info(ss.str());
						position.x = prevPos.x + mouse.pos.x;
						position.y = prevPos.y + mouse.pos.y;
						prevPos.x = mouse.pos.x;
						prevPos.y = mouse.pos.y;
					}
					else {
						Container::handleMouseMove(mouse);
					}
				}

				bool isInDragZone(Mouse& mouse) {
					return mouse.pos.y <= y() + height() && mouse.pos.y >= y() + height() - titleBarHeight;
				}

				void updateState(Mouse& mouse) {
					if (mouse.left.status == Mouse::Button::PRESSED) {
						if (state == RESTING) {
							prevPos.x = mouse.pos.x;
							prevPos.y = mouse.pos.y;
							state = DRAGGING;
							Logger::get("Frame").info("mouse pressed");
						}
					}
					else if (mouse.left.status == Mouse::Button::RELEASED) {
						Logger::get("Frame").info("mouse released");
						state = RESTING;
						prevPos = glm::ivec2(0);
					}
				}

			protected:
				virtual void renderBorder() override {
					glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);

					transformation = glm::translate(pTransform, { x() - 1, y() + titleBarHeight - 1, 0 });
					transformation = glm::scale(transformation, { width() + 2, height() + 2, 1 });

					glm::mat4 mvp = UI::projection * transformation;
					shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
					shader.sendUniform4fv("color", 1, glm::value_ptr(theme.border));
					canvas->draw(shader);
				}

			private:
				const char* title;
				int titleBarHeight;
				Font* font;
				State state;
				glm::ivec2 prevPos = glm::ivec2(0);
			};

		}
	}
}