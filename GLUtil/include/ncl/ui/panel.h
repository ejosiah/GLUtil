#pragma once

#include "component.h"

namespace ncl {
	namespace ui {
		class Panel : public Composite {
		public:
			Panel(int width, int height, gl::Scene& scene, std::string label = "", glm::vec2 pos = glm::vec2(0))
				:Composite(scene, label, pos, width, height) {
				font_pos = glm::vec2(border, _height + font->maxHeight * 0.25 - border);
			}

			virtual void init() {
				gl::Mesh m;
				m.positions.push_back(glm::vec3(0));
				m.positions.push_back(glm::vec3(_width, 0, 0));
				m.positions.push_back(glm::vec3(_width, _height, 0));
				m.positions.push_back(glm::vec3(0, _height, 0));
				m.colors = std::vector<glm::vec4>{ 4, fgColor };
				m.primitiveType = GL_LINE_LOOP;

				outline = new gl::ProvidedMesh(m);

				m.clear();
				m.positions.push_back(glm::vec3(0));
				m.positions.push_back(glm::vec3(_width, 0, 0));
				m.positions.push_back(glm::vec3(_width, _height, 0));
				m.positions.push_back(glm::vec3(0, _height, 0));
				m.colors = std::vector<glm::vec4>{ 4, bgColor };
				m.primitiveType = GL_TRIANGLE_FAN;
				bg = new gl::ProvidedMesh(m);

				m.clear();
				float x = float(border);
				float y = _height - border;
				float w = font->sizeOf(_label).x + (3 * padding);
				float h = font->maxHeight + y;
				m.positions.push_back(glm::vec3(x, y, 0));
				m.positions.push_back(glm::vec3(w, y, 0));
				m.positions.push_back(glm::vec3(w, h, 0));
				m.positions.push_back(glm::vec3(x, h, 0));
				m.colors = std::vector<glm::vec4>{ 4, bgColor };
				m.primitiveType = GL_TRIANGLE_FAN;
				font_bg = new gl::ProvidedMesh(m);

			};

			virtual void render(gl::Shader& shader) {
				float w = _width - border;
				float h = _height - border;
				float sx = w / _width;
				float sy = h / _height;
				float x = (_width - w) / 2;
				float y = (_height - h) / 2;

				cam.model = xform;
				send(cam);
				if (_opaque) {
					shade(font_bg);
				}


				cam.model = xform * glm::translate(glm::mat4(1), { x, y, 0.0f }) * glm::scale(glm::mat4(1), glm::vec3{ sx,  sy, 0.0f });
				send(cam);
				shade(outline);


				if (_opaque) {
					cam.model = xform;
					send(cam);
					shade(bg);
				}
			};

			virtual void renderFont() override {
				glm::vec2 pos = glm::vec2(xform * glm::vec4{ border + padding, _height + font->maxHeight * 0.25 - border, 0, 1 });
				font->render(_label, pos.x, scene.height() - pos.y);
			}


		protected:
			gl::ProvidedMesh * outline;
			gl::ProvidedMesh* bg;
			gl::ProvidedMesh* font_bg;
			int padding = 10;
			glm::vec2 font_pos;
		};
	}
}