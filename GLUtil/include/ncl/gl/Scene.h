#pragma once

#include <map>
#include <algorithm>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gl/gl_core_4_5.h>
#include <initializer_list>
#include "input.h"
#include "logger.h"
#include "models.h"
#include "primitives.h"
#include "Shader.h"
#include "shader_binding.h"
#include "shaders.h"
#include "primitives.h"
#include "Image.h"
#include "Font.h"
#include "common.h"
#include "textures.h"
#include "TransformFeedBack.h"
#include "Resolution.h"
#include "UserCameraController.h"
#include <filesystem>
#include "FrameBuffer.h"


namespace ncl {
	namespace gl {
		const unsigned MAX_LIGHT_SOURCES = 10;

		struct Options {
			std::vector<std::string> shaders = {};
			GLbitfield fBuffer = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
			glm::ivec2 dimensions = glm::ivec2{ 1920 , 1080 };
			bool requireMouse = false;
			bool hideCursor = false;
			bool useDefaultShader = false;
			bool vSync = true;
			bool fullscreen = false;
		};

		static std::filesystem::path shader_loc[] = {
			"shaders",
			"..\\shaders",
		};

		/** @file Scene.h
		* @breif Defines a 3D scene
		*/
		class Scene {
		public:

			friend class SceneObject;

			/**
			* Scene constructor
			* @param t scene title
			* @param res desired screen resolution
			* @param fullscreen enable fullscreen mode
			* @param vsync enable sync frame rate to screen refresh rate
			* @param fbuffer OpenGL framebuffer settings
			*/
			Scene(const char* t, const Resolution& res, bool fullscreen = false, bool vsync = true, GLbitfield fBuffer = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) :
				Scene(t, res.width, res.height, fullscreen, vsync, fBuffer){}

			/**
			* Scene constructor
			* @param t scene title
			* @param w scene width
			* @param h scene height
			* @param fullscreen enable fullscreen mode
			* @param vsync enable sync frame rate to screen refresh rate
			* @param fbuffer OpenGL framebuffer settings
			*/
			Scene(const char* t, int w = 1280, int h = 720, bool fullscreen = false, bool vsync = true, GLbitfield fBuffer = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
			:_width(w), _height(h), _title(t), fBuffer(fBuffer)
			, _fullScreen(fullscreen), _vsync(vsync) {
				_center = glm::vec2(w / 2, h / 2);
				_motionEventHandler = nullptr;
				if (fullscreen) {
					_width = GetSystemMetrics(SM_CXSCREEN);
					_height = GetSystemMetrics(SM_CYSCREEN);
				}
				addShader("flat", GL_VERTEX_SHADER, identity_vert_shader);
				addShader("flat", GL_FRAGMENT_SHADER, identity_frag_shader);
			//	addShader("flat", GL_GEOMETRY_SHADER, viewport_point_glsl_shader);
				addShader("old_default", GL_VERTEX_SHADER, per_fragment_lighing_vert_shader);
				addShader("old_default", GL_FRAGMENT_SHADER, per_fragment_lighing_frag_shader);
			}

			/**
			* Scene constructor with options
			* @param t scene title
			* @param ops scene options
			*/
			Scene(const char* t, Options ops) : Scene(t, ops.dimensions.x, ops.dimensions.y, ops.fullscreen, ops.vSync, ops.fBuffer) {
				_requireMouse = ops.requireMouse;
				_hideCursor = ops.hideCursor;
			}

			/**
			* scene cleanup 
			*/
			virtual ~Scene() {
			}

			/**
			* @breif Private scene initializer
			*/
			void init0(){
				setForeGroundColor(BLACK);
				setBackGroundColor(WHITE);
				using namespace std;
				if (_useImplictShaderLoad) {
					loadShaderImplicity();
				}
				vector<ShaderSource> sources;
			//	sources.push_back(ShaderSource{ GL_VERTEX_SHADER,  per_fragment_lighting2_vert_shader , "default.vert" });
			//	sources.push_back(ShaderSource{ GL_GEOMETRY_SHADER,  wireframe_geom_shader , "default.geom" });
				sources.push_back(ShaderSource{ GL_FRAGMENT_SHADER,  per_fragment_lighting2_frag_shader , "default.frag" });
				_sources.insert(make_pair("default", sources));
				
				for (auto& entry : _sources) {
					Shader* shader = new Shader;

					for (auto source : entry.second) {
						shader->load(source);
					}
					shader->createAndLinkProgram();
					_shaders.insert(std::make_pair(entry.first, shader));
				}

				light[0].on = true;
				lightModel.twoSided = false;
				lightModel.colorMaterial = false;
				glClearColor(1, 1, 1, 1);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				init();
				initDefaultCamera();
				initCameras();

				_keyListeners.push_back([&](const Key& key) {
					if (cameraControlActive) {
						getActiveCameraController().processUserInput();
					}
					processInput(key);
				});
				sFont = Font::Arial(_fontSize, 0, getForeGround());				
			}

			void loadShaderImplicity() {
				if (!_useImplictShaderLoad) return;

				using namespace std;
				using namespace std::filesystem;

				for (auto& p : shader_loc) {
					if (exists(p) && is_directory(p) && !p.empty()) {
						for (auto& entry : directory_iterator(p)) {
							auto& path = entry.path();
							string ext = path.filename().extension().string();
							string filename = path.filename().string();
							auto i = filename.find(ext);
							string name = filename.substr(0, i);
							if (!is_directory(path)) {
								string filename = path.string();
								if (Shader::isShader(filename)) {
									ensureSources(name);
									ShaderSource source = Shader::extractFromFile(filename);
									_sources[name].push_back(source);
									implicityLoaded = true;
								}
							}
						}
					}
				}
			}

			/** 
			 * @breif intialization routine run after the OpenGL context has  been
			 * created
			 */
		 	virtual void init() {

			}

			/** 
			 * @breif called after the window has been resized
			*/
			virtual void resized() {
				
			}


			/**
			* @breif intial display
			*/
			void display0() {
				glClear(fBuffer);
				display();
				if (camInfoOn) {
					sbr << "Camera Settings:" << std::endl;
					sbr << "\tType: " << activeCamera().modeAsString() << std::endl;;
					sbr << "\tPosition: " << activeCamera().getPosition() << std::endl;
					sbr << "\tVelocity: " << activeCamera().getVelocity() << std::endl;

					sFont->render(sbr.str(), 10, 10);
					sbr.str("");
					sbr.clear();
				}

			}

			void update0(float elapsedTime) {
				updateFrameRate(elapsedTime);
				update(elapsedTime);
				if (cameraControlActive) {
					getActiveCameraController().update(elapsedTime);
				}
			}

			void updateFrameRate(float elapsedTime) {
				static float totalTime = 0.0f;
				static int frames = 0;

				totalTime += elapsedTime;
				if (totalTime >= 1.0f) {
					fps = frames;
					totalTime = 0.0f;
					frames = 0;
				}
				else {
					++frames;
				}
			}

			/**
			* @breif public display 
			*/
			virtual void display() {

			}

			/**
			* @breif update scene before display
			*/
			virtual void update(float elapsedTime) {

			}

			/**
			* @breif process keyboard input
			* @param keyboard handle to keyboard
			*/
			virtual void processInput(const Key& key) {

			}

			/**
			* @return scene with
			*/
			int width() const {
				return _width;
			}

			/**
			* @return scene height
			*/
			int height() const {
				return _height;
			}

			/**
			* @return scene center
			*/
			glm::vec2 center() const {
				return _center;
			}

			/**
			* @return scene title
			*/
			const char* title() const{
				return _title;
			}

			/**
			* @breif check to scene if scene requires mouse use
			*/
			bool requireMouse() const {
				return _requireMouse;
			}

			/**
			* @breif check to see if cursor is hidden on scene
			*/
			bool hideCursor() const {
				return _hideCursor;
			}

			/**
			* @return keyListeners registered on scene
			*/
			const std::vector<KeyListener>& KeyListeners() const {
				return _keyListeners;
			}

			/**
			* @return MouseClickListners registered on scene
			*/
			const std::vector<MouseClickListner>& mouseClickListeners() const {
				return _mouseClickListners;
			}

			/**
			* @return MouseMoveListners registered on scene
			*/
			const std::vector<MouseMoveListner>& mouseMoveListners() const {
				return _mouseMoveListner;
			}

			/**
			* @breif adds KeyListener to scene
			* @param listener 
			*/
			void addKeyListener(KeyListener listener) {
				_keyListeners.push_back(listener);
			}

			/**
			* @breif adds MouseClickListner to scene
			* @param listener
			*/
			void addMouseClickListener(MouseClickListner listener) {
				_mouseClickListners.push_back(listener);
			}

			/**
			* @breif adds MouseMoveListner to scene
			* @param listener
			*/
			void addMouseMouseListener(MouseMoveListner listener) {
				_mouseMoveListner.push_back(listener);
			}

			/**
			* @breif called when scene has been resized
			* @param w width after scene has been resized
			* @param h height after scene has been resized
			*/
			void resize(int w, int h) {
				_width = w;
				_height = h;
				_center = glm::vec2(w / 2, h / 2);
				aspectRatio = float(w) / h;
				Font::onResize(w, h);
				try { resized(); } catch (...) {}
			}

			bool shouldResize(int w, int h) {
				return _width != w || _height != h;
			}

			/**
			* @breif push light settings to shader
			*/
			void sendLights(std::string shaderName) {
				shader(shaderName).sendUniformLights(light, MAX_LIGHT_SOURCES);
			}

			/**
			* @return current viewport of scene
			*/
			glm::mat4 getViewport() {
				float w = width() / 2.0;
				float h = height() / 2.0;

				return glm::mat4(
					glm::vec4(w, 0, 0, 0),
					glm::vec4(0, h, 0, 0),
					glm::vec4(0, 0, 1, 0),
					glm::vec4(w, h, 0, 1)
				);
			}

			/**
			* @breif render text on scene
			* @param x x coordinate of text to render
			* @param y y coordinate of text to render
			* @param text text to render on scene
			* @param font font used to render text
			*/
			void renderText(int x, int y, std::string text, Font* font = Font::Arial(10, 0, glm::vec4(0, 0, 0, 1))) {
				font->render(text, x, y);
			}

			/**
			* @return motionEventHandler registered on scene
			*/
			_3DMotionEventHandler* motionEventHandler() const{
				return _motionEventHandler;
			}

			/**
			  @brief set font size for default font 
			  @param size font size
			*/
			void fontSize(int size) {
				_fontSize = size;
			}

			void useImplictShaderLoad(bool flag) {
				_useImplictShaderLoad = flag;
			}

			void addShader(std::string name, GLenum shaderType, const std::string& source) {
				ensureSources(name);
				_sources[name].push_back(ShaderSource{ shaderType, source, ".shader." + std::to_string(shaderType) });
			}

			void addShaderFromFile(std::string name, const std::string& filename) {
				ensureSources(name);
				ShaderSource source = Shader::extractFromFile(filename);
				_sources[name].push_back(source);
			}

			void ensureSources(std::string name) {
				auto itr = _sources.find(name);
				if (itr == _sources.end()) _sources.insert(std::make_pair(name, std::vector<ShaderSource>()));
			}

			bool fullScreen() { return _fullScreen;  }
			bool vSync() { return _vsync;  }

			Shader& shader(std::string name) { return *_shaders[name]; }

			void forShaders(std::initializer_list<std::string> names, Procedure proc) {
				for (std::string name : names) {
					shader(name)(proc);
				}
			}

			/**
			* @breif Sets the background of the scene
			* @param color color to set the background to
			*/
			void setBackGroundColor(const glm::vec4& color) {
				_background = color;
				glClearColor(color.r, color.g, color.b, color.a);
			}

			glm::vec4 getBackground() {
				return _background;
			}

			void setForeGroundColor(const glm::vec4& color) {
				_foreground = color;
			}

			glm::vec4 getForeGround() {
				return _foreground;
			}

			glm::vec3 mousePositionInScene(glm::mat4 model, glm::mat4 projection) {
				Mouse& mouse = Mouse::get();
				float winZ = 0;
				float x = mouse.pos.x;
				float y = _height - mouse.pos.y;
				glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
				return unProject(glm::vec3(x, y, winZ), model, projection, glm::vec4(0, 0, _width, _height));
			}

			void add(CameraController& camera) {
				cameras.push_back(&camera);
			}

			void setActiveCamera(int id) {
				assert(id >= 0 && id < cameras.size());
				activeCam = id;
			}

			void initDefaultCamera() {
				if (cameras.empty()) {
					CameraController* cameraController = new CameraController{ Mesurements{ float(_width), float(_height) }, Camera::SPECTATOR };

					cameraController->setModelHeight(_modelHeight);
					cameraController->setFloorMeasurement({ 10000,  10000, 10000 });

					cameraController->init();
					cameraController->getCamera().setVelocity(5, 5, 5);
					cameraController->getCamera().setAcceleration(glm::vec3(6.0f));
					cameraController->getCamera().setRotationSpeed(0.01f);
					add(*cameraController);

					//CameraSettings settings;
					//settings.sceneDimentions = Mesurements{ float(_width), float(_height) };
					//settings.mode = Camera::SPECTATOR;
					//settings.modelHeight = _modelHeight;
					//settings.velocty = { 5, 5, 5 };
					//settings.acceleration = { 6, 6, 6 };
					//settings.orbitRollSpeed = 0.01f;

					//addCamera(settings);
				}
			}

			void initCameras() {
				//for (auto camera : cameras) camera->init();
			}

			void addCamera(const CameraSettings& settings) {
				CameraController* camCtrl = new CameraController(settings);
				add(*camCtrl);
			}

			void updateCamera(int index, const CameraSettings& settings) {
				assert(index < cameras.size());
				// TODO update camera
			}

			CameraController& getActiveCameraController() {
				return *cameras[activeCam];
			}

			Camera& activeCamera() const {
				return (*cameras[activeCam]).getCamera();
			}

			void deactivateCameraControl() {
				cameraControlActive = false;
			}

			void activateCameraControl() {
				cameraControlActive = true;
			}

			GlmCam cam;
			_3DMotionEventHandler* _motionEventHandler;


		protected:
			int _width;
			int _height;
			const char* _title;
			std::map<std::string, std::vector<ShaderSource>> _sources;
			std::map <std::string, Shader*> _shaders;
			GLbitfield fBuffer;
			bool _requireMouse = true;
			bool _hideCursor = true;
			float aspectRatio;
			glm::vec2 _center;
			std::vector<KeyListener> _keyListeners;
			std::vector<MouseClickListner> _mouseClickListners;
			std::vector<MouseMoveListner> _mouseMoveListner;
			std::vector<CameraController*> cameras;
			LightSource light[MAX_LIGHT_SOURCES];
			LightModel lightModel;
			bool _useImplictShaderLoad = false;
			bool implicityLoaded = false;
			bool _fullScreen = false;
			bool _vsync = true;
			glm::vec4 _background;
			glm::vec4 _foreground;
			int activeCam = 0;
			float fps;
			bool cameraControlActive = true;
			bool camInfoOn = false;
			Font* sFont;
			int _fontSize = 10;
			float _modelHeight = 3;
			std::stringstream sbr;
		};
	}
}