#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/ui/ui.h"
#include "../GLUtil/include/ncl/units/units.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace unit;

class LightingScene;
const string USERNAME = getEnv("username");

enum State { LIGHT_CONTROL, LIGHT_PROPS, CAMERA_CONTROL };


class Object {
public:
	Object(string path, float size, Scene& scene, vec3 position = vec3(0)):scene(scene) {
		
		model = new Model(path, true, size);
		position.y = (model->bound->max().y - model->bound->min().y) / 2;
		local = translate(mat4(1), position);
		lightModel.useObjectSpace = false;
		lightModel.localViewer = true;
	}

	void draw() {
		auto& camera = scene.activeCamera();
		send(camera, local);
		send(lightModel);
		shade(model);
	}

private:
	Model * model;
	mat4 local;
	Scene& scene;
	LightModel lightModel;
};

class LightUI {
public:
	LightUI(LightSource& light, Scene& scene, State& state, bool& gammaCorrect) 
		:light(light), scene(scene), state(state), gammaCorrect(gammaCorrect) {
		using namespace ui;
		visible = true;
		float pWidth = 270;
		float pHeight = 720;
		panel = new ui::Panel(pWidth, pHeight, scene, "Light Settings", { scene.width() - pWidth, scene.height() - pHeight });
		panel->isOpaque(true);
		//	panel->setForGroundColor(getForeGround());
		panel->init();

		directional = new RadioButton(scene, "Directional");
		positional = new RadioButton(scene, "Positional");
		spot = new RadioButton(scene, "Spot");

		if (light.position.w > 0) {
			if (light.spotAngle < 90) {
				spot->select();
			}
			else {
				positional->select();
			}
		}
		else {
			directional->select();
		}

		radioGroup = new ui::RadioGroup(250, 60, scene, "Light Type");
		radioGroup->addRadioButton(directional);
		radioGroup->addRadioButton(positional);
		radioGroup->addRadioButton(spot);
		radioGroup->pos({ 10, panel->height() - 80 });
		radioGroup->init();
		panel->addChild(radioGroup);


		{

			l_panel = new Panel(250, 150, scene, "color", { 10, radioGroup->pos().y - 170 });
			l_panel->init();
			r = new ui::LineSlider(scene, 200, "Red", light.diffuse.r);
			g = new ui::LineSlider(scene, 200, "Green", light.diffuse.g);
			b = new ui::LineSlider(scene, 200, "Blue", light.diffuse.b);
			power = new ui::LineSlider(scene, 200, "Intensity", 1.0f);

			float h = l_panel->height();
			r->pos({ 30, h - 30 });
			b->pos({ 30, h - 60 });
			g->pos({ 30, h - 90 });
			power->pos({ 30, h - 120 });

			r->init();
			g->init();
			b->init();
			power->init();


			scene.addMouseMouseListener([&](Mouse& mouse) {
				if (panel->isVisible()) {
					if (power->active) {
						r->move(power->value());
						g->move(power->value());
						b->move(power->value());
					}
				}
			});

			l_panel->addChild(r);
			l_panel->addChild(g);
			l_panel->addChild(b);
			l_panel->addChild(power);
			panel->addChild(l_panel);

		};

		{
			da_panel = new Panel(250, 120, scene, "Distance attenuation", { 10, l_panel->pos().y - 130 });
			da_panel->init();
			float h = da_panel->height();

			kc = new ui::LineSlider(scene, 200, "kc", light.kc);
			ki = new ui::LineSlider(scene, 200, "ki", light.ki);
			kq = new ui::LineSlider(scene, 200, "kq", light.kq);

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
			spot_panel = new Panel(250, 200, scene, "Spot Settings", { 10, da_panel->pos().y - 220 });
			spot_panel->init();
			float h = spot_panel->height();
			arcSlider = new ArcSlider(scene, "Spot Angle", 30, 90, 100.0f, { 30, 70 });
			//	arcSlider->setForGroundColor(getForeGround());
			arcSlider->init();

			directional->onSelect([&]() {
				light.position.w = 0;
				light.spotAngle = 180;
				// TODO disable spot light control
			});

			positional->onSelect([&] {
				light.position.w = 1;
				light.spotAngle = 180;
				// TODO disable spot light control
			});

			spot->onSelect([&] {
				light.position.w = 1;
				light.spotAngle = arcSlider->angle;
				// enable spot light control;
			});

			lineSlider = new ui::LineSlider(scene, 200, "Spot attenuation", light.spotExponent, 0, 128);
			lineSlider->pos(vec2{ 30 , arcSlider->pos().y - 30 });
			//	lineSlider->setForGroundColor(getForeGround());
			lineSlider->init();

			spot_panel->addChild(arcSlider);
			spot_panel->addChild(lineSlider);
			panel->addChild(spot_panel);
		}

		gammaOn = new RadioButton(scene, "On");
		gammaOff = new RadioButton(scene, "off");

		gammaOn->onSelect([&]() { gammaCorrect = true; });
		gammaOff->onSelect([&]() { gammaCorrect = false;});
		gammaRadioGroup = new RadioGroup(250, 60, scene, "Gamma Correction");
		gammaRadioGroup->addRadioButton(gammaOn);
		gammaRadioGroup->addRadioButton(gammaOff);
		gammaRadioGroup->pos({ 10, spot_panel->pos().y - 80 });
		gammaRadioGroup->init();
		panel->addChild(gammaRadioGroup);
	}

	void hide() {
		panel->hide();
	}

	void show() {
		panel->show();
	}

	void update(float dt) {
		if (state == LIGHT_PROPS) {
			light.diffuse.r = r->value();
			light.diffuse.g = g->value();
			light.diffuse.b = b->value();
			light.specular.r = r->value();
			light.specular.g = g->value();
			light.specular.b = b->value();
			
			light.kc = kc->value();
			light.ki = ki->value();
			light.kq = kq->value();

			if (spot->selected()) {
				light.spotAngle = arcSlider->value();
				light.spotExponent = lineSlider->value();
			}
			
		}
	}

	void draw() {
		if (visible) {
			panel->draw();
		}
	}

public:
	ui::ArcSlider * arcSlider;
	ui::LineSlider* lineSlider;
	ui::Panel* panel;
	ui::Panel* l_panel;
	ui::LineSlider* r;
	ui::LineSlider* g;
	ui::LineSlider* b;
	ui::LineSlider* power;

	ui::Panel* da_panel;
	ui::LineSlider* kc;
	ui::LineSlider* ki;
	ui::LineSlider* kq;

	ui::RadioButton* directional;
	ui::RadioButton* positional;
	ui::RadioButton* spot;
	ui::RadioGroup* radioGroup;

	ui::RadioButton* gammaOn;
	ui::RadioButton* gammaOff;
	ui::RadioGroup* gammaRadioGroup;
	bool& gammaCorrect;

	ui::Panel* spot_panel;
	LightSource& light;
	Scene& scene;
	bool visible;
	State& state;
};

class LightController : public _3DMotionEventHandler {
public:
	LightController(vec4& pos, State& state) :pos{ pos }, state{ state } {
		velocity = vec3(5.0f);
	}

	Logger logger = Logger::get("3D Motion Event");

	virtual void onMotion(const _3DMotionEvent& event) override {
		if(state == LIGHT_CONTROL) direction = clamp(event.translation, vec3(-1), vec3(1));
	};
	virtual void onNoMotion() override {
		direction = vec3(0);
	};

	virtual void update(float dt) {
		auto v = velocity * direction;
		vec3 newP = pos.xyz + v * dt;
		pos = vec4(newP, pos.w);
	}

public:
	vec4 & pos;
	vec3 direction;
	vec3 velocity;
	State& state;
};


class Light {
public:
	Light(LightSource& source, Scene& scene, State& state, bool& gammaCorrect) : source(source), scene(scene) {
		sphere = new Sphere(10.0_in, 10, 10, WHITE);
		lightPanel = new LightUI(source, scene, state, gammaCorrect);
		lightPanel->hide();		
	}

	friend class LightingScene;

	void draw() {
		
		scene.shader("flat")([&] {
			mat4 model = translate(mat4(1), vec3(source.position));
			send(scene.activeCamera(), model);

			shade(sphere);
		});
		lightPanel->draw();
	}

	void update(float dt) {
		lightPanel->update(dt);
		auto n = sphere->numVertices();
		sphere->update2<vec4>(VAOObject::Color, [&](vec4* color) {
			for (int i = 0; i < n; i++) {
				*color = source.diffuse;
				++color;
			}
		});
	}

public:
	LightSource & source;
	Sphere* sphere;
	Scene& scene;
	LightUI* lightPanel;
};


class LightingScene : public Scene {
public:
	LightingScene():Scene("Light models", Resolution::FHD){
		state = LIGHT_CONTROL;
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\stanford_buddha\\buddha.obj");
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\stanford-dragon\\stanford-dragon.obj");
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\Armadillo.obj");
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\Game_model\\Game_model.obj");
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\ChineseDragon.obj");
		paths.push_back("C:\\Users\\Josiah\\OneDrive\\media\\models\\Lucy-statue\\metallic-lucy-statue-stanford-scan.obj");
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
		addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);
		camInfoOn = true;
		
	}

	void init() override {
		initDefaultCamera();
	//	texture = new CheckerTexture(1, "diffuseMap");
		floor = new Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\textures\\GroundForest003\\3K\\GroundForest003_COL_VAR2_3K.jpg", 1, "diffuseMap");
		specular = new Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\textures\\GroundForest003\\3K\\GroundForest003_GLOSS_3K.jpg", 2, "specularMap");
		normal = new Texture2D("C:\\Users\\Josiah\\OneDrive\\media\\textures\\GroundForest003\\3K\\GroundForest003_NRM_3K.jpg", 3, "normalMap");
		
	//	checkerboard = new CheckerBoard_gpu(256, 256, WHITE, GRAY, 1, "diffuseMap");
	//	checkerboard->compute();
	//	checkerboard->images().front().renderMode();

		//shader("default")([&] {
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D, checkerboard->images().front().buffer());
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//});
	//	skybox = new Cube(50, 10, WHITE, false);
		plane = new Plane({ { 0, 1, 0 }, 0 }, 100, 100, 100.0_ft, 100.0_ft, 10, WHITE);
		plane->material().diffuseMat = floor->bufferId();   //checkerboard->images().front().buffer();
		plane->material().specularMat = specular->bufferId();
		plane->material().shininess = 10;

		setBackGroundColor(BLACK);
		setForeGroundColor(WHITE);
		light[0].on = true;
		light[0].position = { 0, 10.0_ft, 0, 1 };
		light[0].spotDirection = { 0, -1, 0, 0 };
		light0 = new Light(light[0], *this, state, gammaCorrect);

		lightModel.localViewer = false;	// TODO per object settings
		lightModel.useObjectSpace = true;

		activeCamera().lookAt({ 0, 1, 1 }, vec3(0), { 0, 1, 0 });
		
		lightController = new LightController(light[0].position, state);

		auto motionHandlers = vector<_3DMotionEventHandler*>{
			lightController,
			light0->lightPanel->arcSlider
		};

		_3dMotionHandler = new Chain3DMotionEventHandler(motionHandlers);
		_motionEventHandler = _3dMotionHandler;

		addMouseClickListener([&](Mouse& mouse) {
			if (!cameraControlActive) {
				if (mouse.left.isPressed()) {
					vec3 point = mousePositionInScene(activeCamera().getViewMatrix(), activeCamera().getProjectionMatrix());
					float minDist = 1000;
					float d = distance(vec3(light[0].position), point);
					if (d < 1) {
						light0->lightPanel->show();
						state = LIGHT_PROPS;
					}
				}
				else if(mouse.right.isPressed()) {
					light0->lightPanel->hide();
					state = LIGHT_CONTROL;
				}
			}
	
		});
		createObjects();
		initSkyBox();
	}

	void createObjects() {
		auto n = paths.size();
		auto step = two_pi<float>() / n;
		float r = 10.0_ft;
		for (int i = 0, theta = 0; i < n; i++, theta += step) {
			float x = r * cos(theta);
			float y = r * sin(theta);
			auto position = vec3(x, 0, y);
			auto object = new Object(paths[i], 6.0_ft, *this, position);
			objects.push_back(object);
		}
	}

	void initSkyBox() {
		createSkyBox();
		string root = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path){
			return root + path;
		});

		skybox = SkyBox::create(skyTextures, 7, *this, 1);
	}

	void createSkyBox() {
		Mesh m;
		m.positions = {
			vec3(-1.0f,  1.0f, -1.0f),
			vec3(-1.0f, -1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f),
			vec3(1.0f,  1.0f, -1.0f),
			vec3(-1.0f,  1.0f, -1.0f),

			vec3(-1.0f, -1.0f,  1.0f),
			vec3(-1.0f, -1.0f, -1.0f),
			vec3(-1.0f,  1.0f, -1.0f),
			vec3(-1.0f,  1.0f, -1.0f),
			vec3(-1.0f,  1.0f,  1.0f),
			vec3(-1.0f, -1.0f,  1.0f),

			vec3(1.0f, -1.0f, -1.0f),
			vec3(1.0f, -1.0f,  1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(1.0f,  1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f),

			vec3(-1.0f, -1.0f,  1.0f),
			vec3(-1.0f,  1.0f,  1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(1.0f, -1.0f,  1.0f),
			vec3(-1.0f, -1.0f,  1.0f),

			vec3(-1.0f,  1.0f, -1.0f),
			vec3(1.0f,  1.0f, -1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(1.0f,  1.0f,  1.0f),
			vec3(-1.0f,  1.0f,  1.0f),
			vec3(-1.0f,  1.0f, -1.0f),

			vec3(-1.0f, -1.0f, -1.0f),
			vec3(-1.0f, -1.0f,  1.0f),
			vec3(1.0f, -1.0f, -1.0f),
			vec3(1.0f, -1.0f, -1.0f),
			vec3(-1.0f, -1.0f,  1.0f),
			vec3(1.0f, -1.0f,  1.0f)
		};
		transform(m.positions.begin(), m.positions.end(), m.positions.begin(), [](vec3 v) { return v * 50.0f; });
		m.colors = vector<vec4>{ m.positions.size(), WHITE };
		m.primitiveType = GL_TRIANGLES;
		skybox2 = new ProvidedMesh(m);
	}

	void display() override {
		if (!cameraControlActive) {
			Mouse::get()._recenter = false;
		}
		
		light0->draw();
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader("default")([&] {
			send("gammaCorrect", gammaCorrect);
			send(lightModel);
			send(light[0]);
			send(activeCamera());

		//	send("material[0].diffuseMap", 1);
			
			//send(floor);
		//	send(specular);
			send(normal);
			shade(plane);
			
			send("uvMappedToSize", false);

			for (auto obj : objects) obj->draw();
		});
		skybox->render();
	}

	void update(float dt) override {
		lightController->update(dt);
		light0->update(dt);
	}

	void resized() override {
		activeCamera().perspective(60.f, aspectRatio, 1.0_m, 1.0_km);
	}

	void processInput(const Key& key) override {
		if (key.value() == 'x') {
			deactivateCameraControl();
			_hideCursor = false;
			_requireMouse = true;
		}
		if (key.value() == 'X') {
			activateCameraControl();
			Mouse::get().recenter();
			Mouse::get().relativePos = vec2(0);
			_hideCursor = true;
		}
	}

private:
	//Cube * skybox;
	ProvidedMesh* skybox2;
	Texture2D * floor;
	Texture2D* normal;
	Texture2D* specular;
	CheckerBoard_gpu* checkerboard;
	Plane* plane;
	Light* light0;
	LightController* lightController;
	Chain3DMotionEventHandler* _3dMotionHandler;
	State state;
	bool gammaCorrect;
	vector<Object*> objects;
	vector<string> paths;
	GLuint skyBoxId;
	SkyBox* skybox;
	vector<string> skyTextures = vector<string>{
		"right.jpg", "left.jpg",
		"top.jpg", "bottom.jpg",
		"front.jpg", "back.jpg"
	};
	
};