#pragma once
#include <functional>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/common.h"

namespace calculus {
	
	constexpr float dx = EPSILON;
	constexpr float dy = EPSILON;
	constexpr float dz = EPSILON;

	using Fn = std::function<float(float)>;
	using scalarFn = std::function<float(glm::vec3)>;
	using vectorFn = std::function<glm::vec3(glm::vec3)>;

	inline glm::vec3 vdx(glm::vec3 v) {
		return { v.x + dx, v.y, v.z };
	}

	inline glm::vec3 vdy(glm::vec3 v) {
		return { v.x, v.y + dy, v.z };
	}

	inline glm::vec3 vdz(glm::vec3 v) {
		return { v.x, v.y, v.z + dz };
	}

	inline float dfdx(Fn f, float x) {
		return (f(x + dx) - f(x)) / dx;
	}

	inline float dfdx(scalarFn f, glm::vec3 v) {
		glm::vec3 vdx = { v.x + dx, v.y, v.z };
		return (f(vdx) - f(v)) / dx;
	}

	inline float dfdy(scalarFn f, glm::vec3 v) {
		glm::vec3 vdy = { v.x, v.y + dy, v.z };
		return (f(vdy) - f(v)) / dy;
	}

	inline float dfdz(scalarFn f, glm::vec3 v) {
		glm::vec3 vdz = { v.x , v.y, v.z + dz };
		return (f(vdz) - f(v)) / dz;
	}

	inline glm::vec3 dfdv(vectorFn f, glm::vec3 v) {
		return {
			(f(vdx(v)) - f(v)).x/dx,
			(f(vdy(v)) - f(v)).y/dy,
			(f(vdy(v)) - f(v)).z/dz
		};
	}

	inline scalarFn divergance(vectorFn f) {
		return [=](glm::vec3 v) -> float {
			return glm::dot(dfdv(f, v), glm::vec3(1));
		};
	}

	inline vectorFn gradiant(scalarFn f) {
		return [=](glm::vec3 v) -> glm::vec3 {
			return {
				dfdx(f, v),
				dfdy(f, v),
				dfdz(f, v)
			};
		};
	}

	inline scalarFn laplacian(scalarFn f) {
		return divergance(gradiant(f));
	}
}