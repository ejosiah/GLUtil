#pragma once

#include <map>
#include "../GLUtil/include/ncl/gl/SceneObject.h"

using namespace ncl;
using namespace gl;
using namespace glm;


enum class WeatherMode {
	CloudType, 
	CloudCovarage, 
	Percipitation
};

class InputProcessor {
	virtual void processInput(const Key& key) = 0;
};

class WeatherGenerator : public SceneObject {
public:
	WeatherGenerator(Scene& scene) 
		:SceneObject{ &scene }
	, currentMode(WeatherMode::CloudCovarage)
	{
		init();
	}

	void init() override {
		
	}
	

	void render(bool shadowMode) override {

	}

	void processInput(const Key& key) override {

	}

private:
	WeatherMode currentMode;
	std::map<WeatherMode, InputProcessor*> inputProcessors;
};