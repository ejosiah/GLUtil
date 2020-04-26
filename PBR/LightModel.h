#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SceneObject.h"

class PbrScene;

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;


const int NORMAL_DISTRIBUTION_FUNC = 1 << 0;
const int GEOMETRY_FUNC = 1 << 1;
const int FRENEL = 1 << 2;
const int ALL = NORMAL_DISTRIBUTION_FUNC | GEOMETRY_FUNC | FRENEL;


class LightingM : public SceneObject  {
public:
	LightingM() = default;

	LightingM(Scene* scene, string name);

	virtual void init();

	string name() {
		return _name;
	}

	virtual PbrScene& _scene();

protected:
	string _name;
	Model* model;
	stringstream ss;
	Font* font;

};

class BSDF : public LightingM {
public:
	BSDF() = default;

	BSDF(Scene* scene);

	void init() override;

	void render() override;

	void displayText();

	void update(float t) override;

	void processInput(const Key& key);

	vec3 color() {
		float r = smoothstep(0.6f, 0.8f, step);
		float g = smoothstep(0.0f, 0.4f, step) - smoothstep(0.8f, 1.0f, step);;
		float b = 1 - smoothstep(0.4f, 0.6f, step);
		return { r, g, b };
	}

private:
	int bitfield;
	pbr::ScalarMaterial material;
	float step = 0;
};

#include "LightModel.inl"