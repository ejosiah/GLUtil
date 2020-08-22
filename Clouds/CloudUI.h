#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/ui/ui.h"
#include "Weather.h"

using namespace ncl;
using namespace gl;
using namespace ui;

class CloudUI {
public:
	CloudUI(Weather& weather, Scene& scene) 
		:_weather{ weather } ,
		_scene{ scene }
	{
		float pWidth = 270;
		float pHeight = 250;
		panel = new ui::Panel(pWidth, pHeight, scene, "Weather Settings", { scene.width() - pWidth + 10, scene.height() - pHeight - 10 });
		panel->init();

		cloud_type = new LineSlider{ scene, 200, "cloud type", weather.cloud_type };
		cloud_coverage = new LineSlider{ scene, 200, "cloud coverage", weather.cloud_coverage};
		percipitation = new LineSlider{ scene, 200, "precipitation", weather.percipitation};

		cloud_type->pos({ 30, pHeight - 30 });
		cloud_coverage->pos({ 30, pHeight - 60 });
		percipitation->pos({ 30, pHeight - 90 });


		cloud_type->init();
		cloud_coverage->init();
		percipitation->init();

		scene.addMouseMouseListener([&](Mouse& ) {
			if (cloud_type->active) {
				weather.cloud_type = cloud_type->value();
			}
			if (cloud_coverage->active) {
				weather.cloud_coverage = cloud_coverage->value();
			}
			if (percipitation->active) {
				weather.percipitation = percipitation->value();
			}
		});

		panel->addChild(cloud_type);
		panel->addChild(cloud_coverage);
		panel->addChild(percipitation);


	}

	~CloudUI() {
		delete panel;
	}

	void hide() {
		panel->hide();
	}

	void show() {
		panel->show();
	}

	void render() {
		panel->draw();
	}

private:
	Weather& _weather;
	const Scene& _scene;
	Panel* panel;
	LineSlider* cloud_type;
	LineSlider* cloud_coverage;
	LineSlider* percipitation;
	
};