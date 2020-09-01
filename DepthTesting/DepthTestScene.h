#pragma once

#include <iterator>
#include <vector>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/StorageBufferObj.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class DepthTestScene : public Scene {
public:
	DepthTestScene() :Scene("Depth Test") 
	{
		_requireMouse = true;
		_hideCursor = false;
		addShader("depth_capture", GL_VERTEX_SHADER, identity_vert_shader);
	}

	void init() override {
		cam.view = lookAt({ 0, 0, 20 }, vec3(0), { 0, 1, 0 });
		cam.model = mat4(1);
		cam.projection = perspective(quarter_pi<float>(), aspectRatio, 0.1f, 100.f);
		points.emplace_back(0, 0, 10);
		points.emplace_back(0, 0, 8);
		points.emplace_back(1, 0, 8);
		points.emplace_back(2, 0, 6);
		points.emplace_back(3, 0, 4);
		points.emplace_back(4, 0, 2);
		points.emplace_back(5, 0, 0);
		points.emplace_back(0, 0, 19.5);

		Mesh mesh;
		mesh.primitiveType = GL_POINTS;
		mesh.colors = vector<vec4>{ points.size(), RED };
		mesh.positions = vector<vec3>(begin(points), end(points));
		point_mesh = ProvidedMesh{ mesh };
		point_mesh.defautMaterial(false);

		createNextIndex();
		depth_values = StorageBufferObj<float>{ size_t{points.size()}, 1 };

		glPointSize(5.0);
		setBackGroundColor(BLACK);
		setForeGroundColor(WHITE);
		_fontColor = WHITE;
		_fontSize = 15;
	}

	void createNextIndex() {
		glGenBuffers(1, &next_index);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, next_index);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &startValue);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, next_index);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	void display() override {
		shader("depth_capture")([&] {
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, next_index);
		//	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &startValue);
			depth_values.sendToGPU();
			send(cam);
			shade(point_mesh);
		});
		logPoints();
		
	}

	void resized() override {
		cam.projection = perspective(quarter_pi<float>(), aspectRatio, 0.1f, 100.f);
	}

	void logPoints() {
		sbr.str("");
		sbr.clear();
		xform_points.clear();
		sbr << "World Space points:\n";
		for (auto& p : points) {
			sbr << "\t" << p << "\n";
			auto aPoint = vec4(p, 1.0);
			xform_points.push_back(aPoint);

		}
		sbr << "\n\nProjection Space points:\n";
		std::transform(begin(xform_points), end(xform_points), begin(xform_points), [&](auto& p) {
			auto out_p = cam.projection * cam.view * cam.model * p;
			sbr << "\t" << out_p << "\n";
			return out_p;
			});

		sbr << "\n\nNDC Space Points:\n";
		for (auto& p : xform_points) {
			auto out_p = p / p.w;
			sbr << "\t" << out_p << "\n";
		}
		

		sbr << "\n\nDepth Values\n";
		auto _near = 0.1f;
		auto _far = 100.f;
		for (auto& p : xform_points) {
			auto value = (p.z/p.w) * 0.5 + 0.5;
			sbr << "\t" << value << "\n";
		}

		sFont->render(sbr.str(), 10, 10);
	}

private:
	vector<vec3> points;
	vector<vec4> xform_points;
	ProvidedMesh point_mesh;
	StorageBufferObj<float> depth_values;
	GLuint startValue = 0;
	GLuint next_index;
};