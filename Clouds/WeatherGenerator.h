#pragma once

#include <map>
#include <memory>
#include <sstream>
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/common.h"

using namespace ncl;
using namespace gl;
using namespace glm;


enum class WeatherMode : int_t {
	CloudCovarage = 1,
	CloudType,
	Percipitation,
	All
};

class WeatherModeProcessor {
public:
	virtual void contextInfo(std::stringstream& sbr) = 0;

	virtual void processInput(const Key& key) = 0;

	virtual void sendData() = 0;
};

class AbstractWeatherProcessor : public WeatherModeProcessor {
public:
	virtual void contextInfo(std::stringstream& sbr) override {
		if (currentOption == Option::Seed) {
			sbr << ", current option: seed\n";
		}
		else if (currentOption == Option::Density) {
			sbr << ", current Option: Density\n";
		}
		sbr << "\tseed: " << seed << "\n";
		sbr << "\tdensity: " << density << "\n";
	}

	virtual void sendData() {
		send("bias", bias);
		send("uSeed", seed);
		send("uDensity", density);
	}

	virtual void processInput(const Key& key) override {
		switch (key.value()) {
		case 's':
			currentOption = Option::Seed;
			break;
		case 'd':
			currentOption = Option::Density;
			break;
		}
		switch (currentOption) {
		case Option::Seed:
			switch (key.value()) {
			case '-':
				seed -= 0.001;
				break;
			case '+':
			case '=':
				seed += 0.001;
			}
			seed = glm::clamp(seed, 0.0f, 1.0f);
			break;
		case Option::Density:
			switch (key.value()) {
			case '-':
				density -= 0.01;
				break;
			case '+':
			case '=':
				density += 0.01;
			}
			density = glm::clamp(density, 0.001f, 1.0f);
			break;
		}
	}
protected:
	float bias = 0.0f;
	float seed = rngReal(0, 1, nextSeed())();
	float density = rngReal(0, 1, nextSeed())();

private:
	enum class Option {
		Seed,
		Density,
		Bias,
		Cycle
	};
	Option currentOption = Option::Seed;
};

class CloudCoverageProcessor : public AbstractWeatherProcessor {
public:
	void contextInfo(std::stringstream& sbr) override {
		sbr << "Option: Cloud Coverage";
		AbstractWeatherProcessor::contextInfo(sbr);
	}

	void sendData() override {
		AbstractWeatherProcessor::sendData();
		send("mode", static_cast<int>(WeatherMode::CloudCovarage));
	}

};

class CloudTypeProcessor : public AbstractWeatherProcessor {
public:
	void contextInfo(std::stringstream& sbr) override {
		sbr << "Option: Cloud Type";
		AbstractWeatherProcessor::contextInfo(sbr);
	}
	void sendData() override {
		AbstractWeatherProcessor::sendData();
		send("mode", static_cast<int>(WeatherMode::CloudType));
	}

};

class PrecipitationProcessor : public AbstractWeatherProcessor {
public:
	void contextInfo(std::stringstream& sbr) override {
		sbr << "Option: Precipitation";
		AbstractWeatherProcessor::contextInfo(sbr);
	}
	void sendData() override {
		AbstractWeatherProcessor::sendData();
		send("mode", static_cast<int>(WeatherMode::Percipitation));
	}

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
		quad = ProvidedMesh{ screnSpaceQuad() };
		processors[WeatherMode::CloudCovarage] = new CloudCoverageProcessor;
		processors[WeatherMode::CloudType] = new CloudTypeProcessor;
		processors[WeatherMode::Percipitation] = new PrecipitationProcessor;
		initGenerator();
	}

	void initGenerator() {
		auto wokers = ivec3{ 128, 128, 1 };
		generator = std::make_unique<Compute>(
			wokers,
			std::vector<Image2D>{ Image2D{1024, 1024} },
			&scene().shader("weather"),
			[&] {
				processors[currentMode]->sendData();
			}
		);
		generator->compute();
	}
	

	void render(bool shadowMode = false) override {
		scene().shader("screen")([&] {
			generator->images().front().renderMode();
			glBindTextureUnit(0, generator->images().front().buffer());
			shade(quad);
		});

		sbr.clear();
		sbr.str("");
		processors[currentMode]->contextInfo(sbr);
		scene().renderText(20, 20, sbr.str(), Font::Arial(15, 0, WHITE));
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case '1': 
				currentMode = WeatherMode::CloudCovarage;
				break;
			case '2':
				currentMode = WeatherMode::CloudType;
				break;
			case '3':
				currentMode = WeatherMode::Percipitation;
				break;
			}

			processors[currentMode]->processInput(key);
			generator->compute();
		}
	}

private:
	WeatherMode currentMode;
	ProvidedMesh quad;
	float bias = 0.0f;
	float seed = 0;
	float density = 1;
	std::unique_ptr<Compute> generator;
	std::map<WeatherMode, WeatherModeProcessor*> processors;
	std::stringstream sbr;
};