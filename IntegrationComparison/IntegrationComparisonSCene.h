#pragma once

#include <algorithm>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/AABB.h"
#include "Integrator.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

class IntegrationComparisonScene : public Scene {
public:
	IntegrationComparisonScene():Scene("Integration methods comparison"){
		bounds = geom::bvol::AABB({ -1.0f, -80, 0.0 }, { 15.0f, 600.0f, 0.0f });
	}

	void init() override {
		setBackGroundColor(BLACK);
		setForeGroundColor(WHITE);
		buildGrids();
		compileIntegratorStats();
	}

	void buildGrids() {
		Mesh m;
		m.positions.push_back({ bounds.min().x, 0, 0 });
		m.positions.push_back({ bounds.max().x, 0, 0 });
		m.positions.push_back({ 0, bounds.min().y, 0});
		m.positions.push_back({ 0, bounds.max().y, 0 });
		m.colors = vector<vec4>(4, WHITE);
		m.primitiveType = GL_LINES;
		grids = new ProvidedMesh(m);
	}

	void compileIntegratorStats() {
		Variables v{ {0, 500, 0}, vec3(0), {0, -10, 0}, 0.0f};
		float dt = 1.0f ;
		int itrs = 11;

		ActualIntegrator actual;
		actual.integrate(v, dt, itrs);

		Mesh m;
		auto size = actual.variables.size();
		m.positions.resize(size);
		m.colors = vector<vec4>(size, WHITE);
		m.primitiveType = GL_LINE_STRIP;
		transform(actual.variables.begin(), actual.variables.end(), m.positions.begin(), [](auto& v) { return vec3(v.t, v.position.y, 0.0f); });
		expected = new ProvidedMesh(m);

		m.clear();
		EularIntegrator eularMethod;
		eularMethod.integrate(v, dt, itrs);
		m.positions.resize(size);
		m.colors = vector<vec4>(size, RED);
		m.primitiveType = GL_LINE_STRIP;
		transform(eularMethod.variables.begin(), eularMethod.variables.end(), m.positions.begin(), [](auto& v) { return vec3(v.t, v.position.y, 0.0f); });
		eular = new ProvidedMesh(m);

		m.clear();
		VerletIntegrator verletMethod;
		verletMethod.integrate(v, dt, itrs);
		m.positions.resize(size);
		m.colors = vector<vec4>(size, YELLOW);
		m.primitiveType = GL_LINE_STRIP;
		transform(verletMethod.variables.begin(), verletMethod.variables.end(), m.positions.begin(), [](auto& v) { return vec3(v.t, v.position.y, 0.0f); });
		verlet = new ProvidedMesh(m);
	}

	void display() override {
		shader("flat")([&] {
			send(cam);
			shade(grids);
			shade(expected);
			shade(eular);
			shade(verlet);
		});
	}



	void processInput(const Key& key) override {

	}

	void resized() override {
		cam.projection = ortho(bounds.min().x, bounds.max().x, bounds.min().y, bounds.max().y);
	}

private:
	geom::bvol::AABB bounds;
	ProvidedMesh* grids;
	ProvidedMesh* expected;
	ProvidedMesh* eular;
	ProvidedMesh* verlet;
};