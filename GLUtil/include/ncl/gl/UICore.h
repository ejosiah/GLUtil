#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.h"
#include "input.h"
#include "Shader.h"
#include "shaders.h"
#include "ProvidedMesh.h"

namespace ncl {
	namespace gl {
		namespace ui {

			class Event;
			class ActionEvent;

			class Layout;

			class Theme {
			public:
				color background;
				color foreground;
				color border;
				color bevelTop;
				color bevelBottom;
				color insetTop;
				color insetBottom;

			};

			class SnowWhite : public Theme {
			public:
				SnowWhite() {
					background = WHITE;
					foreground = BLACK;
					border = BLACK;
					bevelTop = glm::vec4(0.8, 0.8, 0.8, 1);
					bevelBottom = BLACK;
					insetTop = BLACK;
					insetBottom = GRAY;
				}
			};

			class DarkNight : public Theme {

			};

			class Component {
			public:
				virtual int x() const {
					return position.x;
				}

				virtual int y() const {
					return position.y;
				}


				virtual void setPosition(int x, int y) {
					position.x = x;
					position.y = y;
				}

				int width() {
					return dimension.x;
				}

				int height() {
					return dimension.y;
				}

				glm::mat4& getTransformation() {
					return transformation;
				}

				void setTransformation(glm::mat4& t) {
					transformation = t;
				}

				void focus() {
					hasFocus = true;
				}

				bool focused() {
					return hasFocus;
				}

				void unFocus() {
					hasFocus = false;
				}

				virtual void setDimensions(int width, int height) {
					dimension.x = width;
					dimension.y = height;
				}

				void setParent(Component* parent) {
					this->parent = parent;
				}

				void setBackground(color& c) {
					backgroud = c;
				}

				void setForground(color& c) {
					forground = c;
				}
				
				virtual bool hit(int _x, int _y) {
					return _x >= x() && _x <= x() + width() && _y >= y() && _y <= y() + height();
				}

				virtual void render() = 0;

				virtual void handleMouseClick(Mouse& mouse) {}
				virtual void handleMouseMove(Mouse& mouse) {}
				virtual void handleKeyPress(const Key&) {}

				virtual void show();
				virtual void dispose();

			protected:
				glm::ivec2 position;
				glm::ivec2 dimension;
				glm::mat4 transformation = glm::mat4(1);
				bool relative = false;
				const char* name;
				bool hasFocus;
				color backgroud;
				color forground;
				Component* parent;
				ProvidedMesh* canvas;
				static Shader shader;
				static Theme theme;
				static glm::mat4 projection;
				
			};

			Shader Component::shader;
			Theme Component::theme;
			glm::mat4 Component::projection;

			class RelativeComponent : public Component {
			public:
				virtual bool hit(int x, int y) {
					auto pos = getRelativePos();
					auto w = width() + pos.x;
					auto h = height() + pos.y;
					return x >= pos.x && x <= w && y >= pos.y && y <= h;
				}


				int absX() const {
					return position.x;
				}

				int absY() const {
					return position.y;
				}

				virtual int x() const override {
					return getRelativePos().x;
				}

				virtual int y() const override {
					return getRelativePos().y;
				}

				glm::ivec2 getRelativePos() const {
				/*	glm::mat4 model = parent ? parent->getTransformation() : glm::mat4(1);
					model = glm::translate(model, { absX(), absY(), 0 });*/

					auto pos = transformation * glm::vec4(0.f, 0.f, 0.f, 1.f);
					return glm::ivec2(pos.x, pos.y);
				}
			};

			class Container : public Component {
			public:
				Container() {
					name = "container";
				}
				void add(Component* comp);

				virtual void render();

				virtual Component* componentAt(int x, int y) {
					for (Component* comp : children) {
						if (comp->hit(x, y)) {
							return comp;
						}
					}
					return nullptr;
				}

				virtual void handleMouseClick(Mouse& mouse) override {
					auto target = componentAt(mouse.pos.x, mouse.pos.y);
					if (target) {
						target->focus();
						target->handleMouseClick(mouse);
					}

				}

				virtual void handleMouseMove(Mouse& mouse) override {
					auto target = componentAt(mouse.pos.x, mouse.pos.y);
					if (target) {
						target->focus();
						target->handleMouseMove(mouse);
					}
					for (Component* comp : children) {
						comp->handleMouseMove(mouse);
					}
				}

				virtual void handleKeyPress(const Key& key) override {
					auto itr = std::find_if(children.begin(), children.end(), [](Component* child) { return child->focused(); });
					if (itr != children.end()) {
						(*itr)->handleKeyPress(key);
					}
				}

				virtual void show() override;

				void setLayout(Layout* layout);

			protected:
				virtual void renderBorder();
				virtual void renderBackground();

			protected:
				std::vector<Component*> children;
				Layout* layout;
			};

			class Icon {
			private:
				unsigned int id;
				const char* path;
			};


			class UI {
			public:
				static std::vector<Component*> comps;
				static Theme theme;
				static Shader shader;
				static glm::mat4 projection;
				static ProvidedMesh* templateCanvas;
				static glm::ivec2 window;

				static void handleMouseClick(Mouse& mouse) {
					Mouse uiMouse = mouse;
					uiMouse.pos.y = window.y - mouse.pos.y;
					Component* comp = componentAt(uiMouse.pos.x, uiMouse.pos.y);
					
					if (comp) {
						Component* focused = focusedComponent();
						if (focused && focused != comp) {
							focused->unFocus();		
						}
						comp->focus();
						comp->handleMouseClick(uiMouse);
						
					}
				}
				static void handleMouseMove(Mouse& mouse) {
					Mouse uiMouse = mouse;
					uiMouse.pos.y = window.y - mouse.pos.y;
					Component* comp = componentAt(uiMouse.pos.x, uiMouse.pos.y);
					if (comp) {
						comp->focus();
						comp->handleMouseMove(uiMouse);
					}
				}
				static void handleKeyPress(const Key& key) {
					Component* comp = focusedComponent();
					if (comp) {
						comp->handleKeyPress(key);
					}
				}

				static void init(Theme& theme = SnowWhite()) {
					UI::theme = theme;
					shader.loadFromstring(GL_VERTEX_SHADER, ui_vert_shader);
					shader.loadFromstring(GL_FRAGMENT_SHADER, ui_frag_shader);
					shader.createAndLinkProgram();

					using namespace glm;

					Mesh mesh;
					mesh.positions.push_back(vec3(0, 1, 0));
					mesh.positions.push_back(vec3(0, 0, 0));
					mesh.positions.push_back(vec3(1, 1, 0));
					mesh.positions.push_back(vec3(1, 0, 0));
					mesh.primitiveType = GL_TRIANGLE_STRIP;

					templateCanvas = new ProvidedMesh(std::vector<Mesh>(1, mesh));
				}

				static void render() {
					for (Component* comp : comps) {
						comp->render();
					}
				}

				static void resize(float width, float height) {
					window.x = width;
					window.y = height;
					projection = glm::ortho(0.0f, width, 0.0f, height);
				}


			protected:
				static Component* componentAt(int x, int y) {
					for (Component* comp : comps) {
						if (comp->hit(x, y)) {
							return comp;
						}
					}
					return nullptr;
				}

				static Component* focusedComponent() {
					auto itr = std::find_if(comps.begin(), comps.end(), [](Component* comp) { return comp->focused(); });
					if (itr != comps.end()) {
						return *itr;
					}
					else {
						return nullptr;
					}
				}
			};

			std::vector<Component*> UI::comps;
			Theme UI::theme;
			Shader UI::shader;
			glm::mat4 UI::projection;
			ProvidedMesh* UI::templateCanvas;
			glm::ivec2 UI::window;

			void Component::show() {
				UI::comps.push_back(this);
				theme = UI::theme;
				shader = UI::shader;
				canvas = UI::templateCanvas;
			}

			void Component::dispose() {
				
			}

			void Container::render() {
				shader.use();
				glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);
				transformation = glm::translate(pTransform, { x(), y(), 0 });
				for (Component* child : children) {
					child->render();
				}
				renderBackground();
				renderBorder();
				shader.unUse();
			}

			void Container::renderBackground() {
				transformation = glm::scale(transformation, { width(), height(), 1 });

				glm::mat4 mvp = UI::projection * transformation;
				shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
				shader.sendUniform4fv("color", 1, glm::value_ptr(theme.background));
				canvas->draw(shader);
			}

			void Container::renderBorder() {
				glm::mat4 pTransform = parent ? parent->getTransformation() : glm::mat4(1);
				
				transformation = glm::translate(pTransform, { x() - 1, y() - 1, 0 });
				transformation = glm::scale(transformation, { width() + 2, height() + 2, 1 });

			//	transformation = glm::translate(pTransform, { x() + 1, y() - 1, 0 });
			//	transformation = glm::scale(transformation, { width(), height(), 1 });

				glm::mat4 mvp = UI::projection * transformation;
				shader.sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(mvp));
				shader.sendUniform4fv("color", 1, glm::value_ptr(theme.border));
				canvas->draw(shader);
			}
		}
	}
}
