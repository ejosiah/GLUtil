#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/glm/vec_util.h"
#include "../GLUtil/include/ncl/ui/ui.h"
#include <glm/glm.hpp>
#include <sstream>
#include <functional>
#include <tuple>


using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace ui;

class SliderScene : public Scene {
public:
	SliderScene():Scene("Sliders", 1280, 1144){
		_requireMouse = true;
		_hideCursor = false;
	}

	void init() override {
		
		setBackGroundColor(BLUE);
		setForeGroundColor(WHITE);
	//	arcSlider = new ArcSlider(*this, 100.f, { width() * 0.333, height() * 0.5, });


		auto half_w = _width / 2;
		auto half_h = _height / 2;
	//	panel = new Panel(270, 640, *this, "Light Settings", { 100, _height - 700 });
		panel = new Panel(270, 640, *this, "Light Settings", { 0, 0 });
		panel->isOpaque(false);
	//	panel->setForGroundColor(getForeGround());
		panel->init();
		

		{

			l_panel = new Panel(250, 150, *this, "color", { 10, panel->height() - 170 });
			l_panel->isOpaque(false);
			l_panel->init();
			r = new LineSlider(*this, 200, "Red", 0.5f);
			g = new LineSlider(*this, 200, "Green", 1.0f);
			b = new LineSlider(*this, 200, "Blue", 0.2f);
			power = new LineSlider(*this, 200, "Intensity", 1.0f);

			float h = l_panel->height();
			r->pos({ 30, h - 30 });
			b->pos({ 30, h - 60 });
			g->pos({ 30, h - 90 });
			power->pos({ 30, h - 120 });

			r->init();
			g->init();
			b->init();
			power->init();

			
			addMouseMouseListener([&](Mouse& mouse){
				if (power->active) {
					r->move(power->value());
					g->move(power->value());
					b->move(power->value());
				}
			});
			
			l_panel->addChild(r);
			l_panel->addChild(g);
			l_panel->addChild(b);
			l_panel->addChild(power);
			panel->addChild(l_panel);
			
		};

		{
			da_panel = new Panel(250, 120, *this, "Distance attenuation", { 10, l_panel->pos().y - 130 });
			da_panel->isOpaque(false);
			da_panel->init();
			float h = da_panel->height();

			kc = new LineSlider(*this, 200, "kc", 1, 0, 10);
			ki = new LineSlider(*this, 200, "ki", 0, 0, 10);
			kq = new LineSlider(*this, 200, "kq", 0, 0, 10);

			kc->pos({ 30, h - 30 });
			ki->pos({ 30, h - 60 });
			kq->pos({ 30, h - 90 });

			kc->init();
			ki->init();
			kq->init();

			da_panel->addChild(kc);
			da_panel->addChild(ki);
			da_panel->addChild(kq);
			panel->addChild(da_panel);
		}
		{
			spot_panel = new Panel(250, 200, *this, "Spot Settings", { 10, da_panel->pos().y - 220 });
			spot_panel->isOpaque(false);
			spot_panel->init();
			float h = spot_panel->height();
			arcSlider = new ArcSlider(*this, "Spot Angle", 100.0f, { 30, 70 });
			arcSlider->angle = 180.0f;
			//	arcSlider->setForGroundColor(getForeGround());
			arcSlider->init();
			_motionEventHandler = arcSlider;

			lineSlider = new LineSlider(*this, 200, "Spot attenuation", 2.0, 0, 10);
			lineSlider->pos(vec2{ 30 , arcSlider->pos().y - 30 });
			//	lineSlider->setForGroundColor(getForeGround());
			lineSlider->init();

			spot_panel->addChild(arcSlider);
			spot_panel->addChild(lineSlider);
			panel->addChild(spot_panel);
		}

		radioGroup = new RadioGroup(250, 60, *this, "Light Type");
		radioGroup->addRadioButton(new RadioButton(*this, "Directional"));
		radioGroup->addRadioButton(new RadioButton(*this, "Positional"));
		radioGroup->pos({ 10, spot_panel->pos().y - radioGroup->height() - 20 });
		radioGroup->isOpaque(false);
		radioGroup->init();
		panel->addChild(radioGroup);

	//	panel->addChild(arcSlider);
	//	panel->addChild(lineSlider);
		

		font = Font::Arial(10, 0, YELLOW);
	}

	void display() override {
		Mouse::get()._recenter = false;
		panel->draw();
	//	l_panel->draw();
	//	arcSlider->render();
		if (arcSlider->active) {
			font->render(" arch slider active", 10, 10);
			font->render(to_string(arcSlider->angle) + " degrees", 10, 20);
		}
		else if(lineSlider->active) {
			font->render("line slider active", 10, 10);
			font->render("value: " + to_string(lineSlider->value()), 10, 20);
		}
		
	}

	void resized() override {
	//	cam.projection = glm::ortho(0.0f, float(_width), 0.0f, float(_height));
		cam.projection = perspective(radians(60.f), aspectRatio, 1.0f, 100.f);
	}

private:
	Font* font;
	stringstream ss;
	ArcSlider* arcSlider;
	LineSlider* lineSlider;
	Panel* panel;
	Panel* l_panel;
	LineSlider* r;
	LineSlider* g;
	LineSlider* b;
	LineSlider* power;

	Panel* da_panel;
	LineSlider* kc;
	LineSlider* ki;
	LineSlider* kq;
	RadioGroup* radioGroup;

	Panel* spot_panel;

};