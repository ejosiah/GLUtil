#pragma once
#include <glm/glm.hpp>

class Field {
public:
	virtual ~Field() = default;
};

class ScalarField : public Field {
public:
	ScalarField() = default;

	virtual ~ScalarField() = default;

	virtual double sample(const glm::vec3& x) const = 0;

	virtual glm::vec3 gradient(const glm::vec3& x) const = 0;

	virtual double laplacian(const glm::vec3& x) const = 0;

};

/*
class VectorField : public Field {
public:
	VectorField();

	virtual ~VectorField();

	virtual glm::vec3 sample(const glm::vec3& x) const = 0;

	virtual double div(const glm::vec3& x) const = 0;

	virtual glm::vec3 curl(const glm::vec3& x) const = 0;
}*/