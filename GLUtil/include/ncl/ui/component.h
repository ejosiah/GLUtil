#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <sstream>
#include <functional>
#include <tuple>
#include "../gl/Scene.h"
#include "../gl/Font.h"
#include "../gl/input.h"

namespace ncl {
	namespace ui {
		class Component {
		public:
			Component(gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0), int w = 0, int h = 0)
				: scene(scene), _label(label), _pos(pos), _width(w), _height(h) {
				local = translate(glm::mat4(1), glm::vec3(pos, 0.0f));
				parent = nullptr;
				bgColor = gl::WHITE;
				fgColor = gl::BLACK;
				font = gl::Font::Arial(10, 0, fgColor);
				// TODO handle on resize event
				cam.projection = glm::ortho(0.0f, float(scene.width()), 0.0f, float(scene.height()));
				visible = true;
			}

			virtual void init() = 0;

			virtual void render(gl::Shader& s) = 0;

			virtual void update(float dt) {}

			virtual void renderFont() {}

			void setParent(Component* parent) {
				this->parent = parent;
			}

			virtual void setBackgroundColor(glm::vec4 color) {
				bgColor = color;
			}

			virtual void setForGroundColor(glm::vec4 color) {
				fgColor = color;
				font = gl::Font::Arial(10, 0, fgColor);
			}

			virtual void setTransform(glm::mat4 xform) {
				this->xform = xform * this->local;
			}

			void pos(glm::vec2 p) {
				_pos = p;
				local = translate(glm::mat4(1), glm::vec3(p, 0));
			}

			glm::vec2 pos() {
				return _pos;
			}

			int width() {
				return _width;
			}

			int height() {
				return _height;
			}

			void show() {
				visible = true;
			}

			void hide() {
				visible = false;
			}

			bool isVisible() {
				return visible;
			}

			void addKeyListener(std::function<void(Component*, const gl::Key&)> listener) {
				scene.addKeyListener([&](const gl::Key& key) {
					listener(this, key);
				});
			}

			void addMouseClickListner(std::function<void(Component*, const gl::Mouse&)> listener) {
				scene.addMouseClickListener([&](gl::Mouse& mouse) {
					listener(this, mouse);
				});
			}

			void addMouseClickListener(std::function<void(Component*, const gl::Mouse&)> listener) {
				scene.addMouseMouseListener([&](gl::Mouse& mouse) {
					listener(this, mouse);
				});
			}

		protected:
			gl::Scene & scene;
			glm::mat4 xform;
			glm::mat4 local;
			Component* parent;
			std::string _label;
			glm::vec2 _pos;
			int _width;
			int _height;
			glm::vec4 bgColor;
			glm::vec4 fgColor;
			gl::Font* font;
			std::stringstream ss;
			gl::GlmCam cam;
			bool visible;
			bool initialized = false;
		};

		class Composite : public Component {
		public:
			Composite(gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0), int w = 0, int h = 0)
				:Component(scene, label, pos, w, h) {
				xform = local;
				_opaque = true;
				border = 10;
			}

			virtual void init() {
				for (auto child : children) child->init();
			}

			void draw() {
				if (isVisible()) {
					for (auto child : children) {
						auto composite = dynamic_cast<Composite*>(child);
						if (composite) {
							composite->draw();
						}
						else {
							scene.shader("flat")([&](gl::Shader& s) {
								child->render(s);
							});
						}
					}

					scene.shader("flat")([&](gl::Shader& s) { render(s);});
					for (auto child : children) {
						auto composite = dynamic_cast<Composite*>(child);
						if (!composite) child->renderFont();
					}
					renderFont();
				}
			}

			void isOpaque(bool flag) {
				_opaque = flag;
			}

			void addChild(Component* child) {
				child->setTransform(xform);
				child->setParent(this);
				children.push_back(child);
			}

			virtual void setTransform(glm::mat4 xform) {
				Component::setTransform(xform);
				for (auto child : children) child->setTransform(this->xform);
			}

			virtual void setBackgroundColor(glm::vec4 color) {
				bgColor = color;
				for (auto child : children) child->setBackgroundColor(color);
			}

			virtual void setForGroundColor(glm::vec4 color) {
				fgColor = color;
				for (auto child : children) child->setForGroundColor(color);
			}

		protected:
			std::vector<Component*> children;
			bool _opaque;
			int border;

		};
	}
}