#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/animation/Animation.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;
using namespace animation;

class AnimationScene : public Scene {
public:
	AnimationScene():Scene("Animation")
	{
		//camInfoOn = true;
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
	}

	void init() override {
		initDefaultCamera();
		activeCamera().setPosition({ 0, 0, 4 });
	//	auto path = "C:\\Users\\joebh\\OneDrive\\level_set_liquid_sim_output";
	//	animation = AnimationBuilder::ObjAnimationBuilder().build(*this, path);
		auto path = "C:\\Users\\joebh\\OneDrive\\smoke_sim_output";
		animation = AnimationBuilder::Image2DAnimationBuilder().build(*this, path);
	}

	void display() override {
		//shader("flat")([&]() {
		//	send(activeCamera());
		//	shade(*animation);
		//});

		shader("screen")([&]() {
			shade(*animation);
		});
	}

	void update(float dt) override {
		animation->update(dt);
	}


private:
	std::unique_ptr<Animation> animation;
};

