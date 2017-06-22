#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gl/gl_core_4_5.h>
#include "input.h"
#include "logger.h"
#include "models.h"
#include "primitives.h"
#include "Shader.h"
#include "shaders.h"
#include "primitives.h"
#include "Image.h"
#include "Font.h"
#include "common.h"
#include "textures.h"

namespace ncl {
	namespace gl {
		const unsigned MAX_LIGHT_SOURCES = 10;

		struct Options {
			std::vector<std::string> shaders = {};
			GLbitfield fBuffer = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
			glm::ivec2 dimensions = glm::ivec2{ 1280, 720 };
			bool requireMouse = false;
			bool hideCursor = false;
			bool useDefaultShader = false;
		};

		/** @file Scene.h
		* @breif Defines a 3D scene
		*/
		class Scene {
		public:
			/**
			* Scene constructor
			* @param t scene title
			* @param w scene width
			* @param h scene height
			* @param fbuffer OpenGL framebuffer settings
			*/
			Scene(const char* t, int w = 1280, int h = 720, bool useDefaultShader = true, std::vector<std::string> shaders = {}, GLbitfield fBuffer = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
			:_width(w), _height(h), _title(t), _useDefaultShader(useDefaultShader), _shaders(shaders),  fBuffer(fBuffer) {
				_center = glm::vec2(w / 2, h / 2);
				_motionEventHandler = nullptr;
			}

			/**
			* Scene constructor with options
			* @param t scene title
			* @param ops scene options
			*/
			Scene(const char* t, Options ops) : Scene(t, ops.dimensions.x, ops.dimensions.y, ops.useDefaultShader, ops.shaders, ops.fBuffer) {
				_requireMouse = ops.requireMouse;
				_hideCursor = ops.hideCursor;
			}

			/**
			* scene cleanup 
			*/
			virtual ~Scene() {
				delete _motionEventHandler;
			}

			/**
			* @breif Private scene initializer
			*/
			void init0(){
				if (_shaders.empty() && _useDefaultShader) {
					_shader.loadFromstring(GL_VERTEX_SHADER, per_fragment_lighing_vert_shader);
				//	_shader.loadFromstring(GL_GEOMETRY_SHADER, wireframe_geom_shader);
					_shader.loadFromstring(GL_FRAGMENT_SHADER, per_fragment_lighing_frag_shader);
					_shader.createAndLinkProgram();
				}
				else if(!_shaders.empty()) {
					_shader.loadFromFiles(_shaders);
					_shader.createAndLinkProgram();
				}
				

				light[0].on = true;
				lightModel.twoSided = false;
				lightModel.colorMaterial = false;
				glClearColor(1, 1, 1, 1);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);

				// TODO enable based on framebuffer

				_shader.use();
				init();

				_keyListeners.push_back([&](const Key& key) {
					processInput(Keyboard::get());
				});
				
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
			* @breif prepare scene before use
			*/
			void prepare() {
				if (_shader.isActive()) {
					_shader.use();
					_shader.send(cam);
					_shader.send(lightModel);
					sendLights();
				}
			}

			/**
			* @breif intial display
			*/
			void display0() {
				glClear(fBuffer);
				display();
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
			virtual void processInput(Keyboard& keyboard) {

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
				resized();
			}

			/**
			* @breif push light settings to shader
			*/
			void sendLights() {
				_shader.sendUniformLights(light, MAX_LIGHT_SOURCES);
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
			void renderText(int x, int y, std::string text, Font* font = Font::Arial(10, 0, glm::vec4(1, 1, 0, 1))) {
				font->render(text, x, y);
			}

			/**
			* @return motionEventHandler registered on scene
			*/
			_3DMotionEventHandler* motionEventHandler() const{
				return _motionEventHandler;
			}

		protected:
			int _width;
			int _height;
			const char* _title;
			bool _useDefaultShader;
			std::vector<std::string> _shaders;
			GLbitfield fBuffer;
			bool _requireMouse = false;
			bool _hideCursor = false;
			float aspectRatio;
			glm::vec2 _center;
			std::vector<KeyListener> _keyListeners;
			std::vector<MouseClickListner> _mouseClickListners;
			std::vector<MouseMoveListner> _mouseMoveListner;
			Shader _shader;
			GlmCam cam;
			LightSource light[MAX_LIGHT_SOURCES];
			LightModel lightModel;
			_3DMotionEventHandler* _motionEventHandler;

		};
	}
}