#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include <limits>
#include <functional>
#include <utility>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

static const int MAX_INT = numeric_limits<int>::max();

class EllipseScene : public Scene {
public:
	EllipseScene(const Resolution& res) :Scene("Ellipse", res) {
		_requireMouse = true;
	}

	void init() override {
		setForeGroundColor(BLACK);
		setBackGroundColor(BLACK);

		rotate90 = mat3(rotate(mat4(1), half_pi<float>(), { 0, 0, 1 }));

		Mesh m;
		float two_pi = 2 * pi<float>();
		float step = two_pi / steps;

		float theta = 0;
		for (int i = 0; i < steps; i++, theta += step) {
			m.positions.push_back({  cos(theta), sin(theta), 0 });
			m.colors.push_back(WHITE);
		}


		m.primitiveType = GL_LINE_LOOP;
		circle = new ProvidedMesh(m);

		m.clear();
		m.positions.push_back(vec3(0));
		m.colors.push_back(RED);
		m.primitiveType = GL_POINTS;
		point = new ProvidedMesh(m);



		m.clear();
		m.positions.resize(steps * 2);
		m.colors = std::vector<vec4>(steps * 2, WHITE);
		m.primitiveType = GL_LINES;
		lines = new ProvidedMesh(m);

		_mouseClickListners.push_back([&](Mouse& mouse) {
			if (mouse.left.status == Mouse::Button::PRESSED) {
				vec3 p = mousePositionInScene();
				point->update2<vec3>(VAOObject::Position, [&p](vec3* oldPos) {
					*oldPos = vec3(p.xy, 0);
				});

				lines->get<vec3>(0, VAOObject::Position, [&](vec3* line) {
					for (int i = 0; i < steps * 2; i++, line++) {
						*line = vec3(MAX_INT);
					}
				});
			}
		});

		glPointSize(5.0f);
	}

	virtual void processInput(const Key& key) {
		vec3 p = point->getFirstVertex();
		if ( key.value() == 257 && key.released() && p.x >= -1.0f && p.x <= 1.0f) {
			
			lines->update2<vec3>(VAOObject::Position, [&](vec3* line) {
				circle->get<vec3>(0, VAOObject::Position, [&](vec3* c) {
					for (int i = 0; i < steps; i++) {
						*line = p; line++;
						*line = *c; line++;
						c++;
					}
				});
			});

		}
		if (key.value() == 't' && key.released() && lines->getFirstVertex().x != MAX_INT) {
			translateLines = true;
		}
	}

	void display() override {
		shader("flat")([&] {
			send(cam);
			shade(circle);
			shade(point);
			shade(lines);
		});
	}

	void update(float dt) override {
		static function<void(vec3&, vec3&, float)> doRotate = [](vec3& a, vec3& b, float t) {
			float l = distance(a, b) * 0.5;
			auto mid = (a + b) * 0.5f;

			mat4 mat = translate(mat4(1), mid);
			mat = rotate(mat, radians(t), { 0, 0, 1 });
			mat = translate(mat, -mid);


			a = (mat * vec4(a, 1.0f)).xyz;
			b = (mat * vec4(b, 1.0f)).xyz;
			l;
		};

		if (translateLines) {
			t += v;
			lines->update2<vec3>(VAOObject::Position, [&](vec3* line) {
				doRotate(*(line + currentLine), *(line + currentLine + 1), v);
			});
			if (t >= 90.0f) {
				t = 0;
				currentLine += 2;
			}
			if (currentLine == steps * 2) {
				t = 0;
				currentLine = 0;
				translateLines = false;
			}

		}
	}

	void resized() override {
		cam.projection = glm::ortho(-1.2f, 1.2f, -1.2f, 1.2f);
	}
private:
	ProvidedMesh * circle;
	ProvidedMesh* point;
	ProvidedMesh* lines;
	bool translateLines = false;
	mat3 rotate90;
	float t = 0;
	float v = 15;
	int currentLine = 0;
	float steps = 100;
};
