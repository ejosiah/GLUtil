#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

struct Variables {
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	float t;
};

class Integrator {
public:
	vector<Variables> variables;

	virtual void integrate(const Variables& intial, float dt, unsigned iterations = 100) = 0;
};

class ActualIntegrator : public Integrator {
public:
	virtual void integrate(const Variables& initial, float dt, unsigned iterations) override {

		auto f = [](vec3 p, vec3 v, vec3 a, float t) -> vec3 {
			return p + (v * t) + (0.5f * a * t * t);
		};
		
		float t = 0;
		for (int i = 0; i < iterations; i++, t += dt) {
			auto p = f(initial.position, initial.velocity, initial.acceleration, t);
			auto v = t > 0 ? (p - initial.position) / t : initial.velocity;
			auto a = t > 0 ? (v - initial.velocity) / t : initial.acceleration;
			variables.push_back({p, v, a, t});
		}
	}
};

class EularIntegrator : public Integrator {
public:
	virtual void integrate(const Variables& initial, float dt, unsigned iterations) override {

		auto p1 = [](vec3 p, vec3 v, float t) -> vec3 {
			return p + v * t;
		};
		auto v1 = [](vec3 v, vec3 a, float t) -> vec3 {
			return v + a * t;
		};

		auto nextT = [&dt](int i) { return i == 0 ? 0 : dt; };

		float t = 0;
		auto prev_variables = initial;
		auto a = prev_variables.acceleration;
		
		for (int i = 0; i < iterations; i++) {
			float dt = nextT(i);
			auto p = p1(prev_variables.position, prev_variables.velocity, dt);
			auto v = v1(prev_variables.velocity, prev_variables.acceleration, dt);
			prev_variables.position = p;
			prev_variables.velocity = v;
			t += dt;
			variables.push_back({ p, v, a, t });
		}
	}
};

class VerletIntegrator : public Integrator {
public:
	virtual void integrate(const Variables& initial, float dt, unsigned iterations) override {

		auto p1 = [](vec3 p0, vec3 p1, vec3 a, float t) -> vec3 {
			return p1 + (p1 - p0) + (a * t * t);
		};
		auto v1 = [](vec3 p0, vec3 p1, float t) -> vec3 {
			return 1/t * (p1 - p0);
		};

		auto nextT = [&dt](int i) { return i == 0 ? 0 : dt; };

		float t = 0;
		auto prev_variables = initial;
		auto a = prev_variables.acceleration;
		auto p0 = initial.position; // initial.position + (0.5f * initial.acceleration * dt * dt);

		for (int i = 0; i < iterations; i++) {
			float dt = nextT(i);
			auto p = p1(p0, prev_variables.position, prev_variables.acceleration, dt);
			auto v = v1(prev_variables.velocity, prev_variables.acceleration, dt);
			p0 = prev_variables.position;
			prev_variables.position = p;
			prev_variables.velocity = v;
			t += dt;
			variables.push_back({ p, v, a, t });
		}
	}
};

class FourthOrderRungeKuttaMethod : public Integrator {
public:
	virtual void integrate(const Variables& initial, float dt, unsigned iterations) override {

	}
};