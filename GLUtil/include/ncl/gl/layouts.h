#pragma once

#include "UICore.h"

namespace ncl {
	namespace gl {
		namespace ui {
			class Layout : public Container {

			};

			class FlowLayout : public Layout {
			public:
				FlowLayout() {
					name = "flow layout";
					setMargin(10);
				}
				virtual void render() override {
					transformation =  parent->getTransformation();
					transformation = glm::translate(transformation, { 0, height() - margin.top, 0 });
					float w = 0.0f;
					float h = 0.0f;
					for (Component* child : children) {
						if (h > height()) break;
						transformation = glm::translate(transformation, { margin.left, -child->height(), 0 });
						child->render();
						transformation = glm::translate(transformation, { child->width(), child->height(), 0 });
						w += child->width() + margin.left;
						if (w >= width()) {
							transformation = glm::translate(transformation, { 0, h + margin.top, 0 });
						}
						
						h = glm::max(h, float(child->height()));
					}
					transformation = parent->getTransformation();
					transformation = glm::translate(transformation, { 0, height() - margin.top, 0 });
				}

				void setMargin(float val) {
					margin = { val, val, val, val };
				}

				void leftMargin(float val) {
					margin.left = val;
				}

				void topMargin(float val) {
					margin.top = val;
				}

				void rightMargin(float val) {
					margin.right = val;
				}

				void buttomMargin(float val) {
					margin.bottom = val;
				}

			protected:
				struct {
					float left, top, right, bottom;
				} margin;
			};

			void Container::add(Component* comp) {
				if (layout) {
					layout->add(comp);
				}else {
					comp->setParent(this);
					children.push_back(comp);
				}
			}

			void Container::setLayout(Layout* layout) {
				this->layout = layout;
				this->layout->parent = this;
				children.push_back(this->layout);
			}

			void Container::show() {
				if (layout) {
					layout->setPosition(x(), y());
					layout->setDimensions(width(), height());
				}
				Component::show();
			}
		}
	}
}