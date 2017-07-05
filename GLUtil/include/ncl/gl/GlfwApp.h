#pragma once

#include <stdexcept>
#include <iostream>
#include <gl/gl_core_4_5.h>
#include <GLFW/glfw3.h>
#include "input.h"
#include "Scene.h"
#include "Timer.h"
#include "logger.h"
#include "Font.h"


#ifdef CONNECT_3D

#include "3DConnexion.h"

#endif
//#include "UICore.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "GLLoader.lib")

namespace ncl {
	namespace gl {

		static glm::vec2 center(0);

		static void onError(int error, const char* description) {

		}

		static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
			static bool localKey = false;
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			const Key* kptr =  Keyboard::get(key);
			if (!kptr) {
				localKey = true;
				kptr = new Key{ {key} };
			}
			if(mods & GLFW_MOD_SHIFT){ 
				kptr->isLowerCase = false;
			}
			else {
				kptr->isLowerCase = true;
			}
			switch (action) {
			case GLFW_PRESS:
			case GLFW_REPEAT:
				kptr->press();
				break;
			case GLFW_RELEASE:
				kptr->release();
				break;
			}
			for (auto listener : keyListeners) {
				listener(*kptr);
			}
			if (localKey) {
				localKey = false;
				delete kptr;
			}
		}

		static void onMouseMove(GLFWwindow* window, double x, double y) {
			using namespace std;
			Mouse& mouse = Mouse::get();
			mouse.pos = glm::vec2(float(x), float(y));
			mouse.relativePos.x += mouse.pos.x - center.x;
			mouse.relativePos.y += center.y - mouse.pos.y;

			for (auto listener : mouseMoveListners) {
				listener(mouse);
			}
		}

		static void buttonStatus(Mouse::Button& button, int action) {
			if (action == GLFW_PRESS) {
				button.status = Mouse::Button::PRESSED;
			}
			else if (action == GLFW_RELEASE) {
				button.status = Mouse::Button::RELEASED;
			}
		}

		static void onMouseClick(GLFWwindow* window, int button, int action, int mods) {
			Mouse& mouse = Mouse::get();
			switch (button){
			case GLFW_MOUSE_BUTTON_LEFT:
				buttonStatus(mouse.left, action);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				buttonStatus(mouse.middle, action);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				buttonStatus(mouse.right, action);
				break;
			}

			for (auto listener : mouseClickListners) {
				listener(mouse);
			}
		}

		struct GLVersion {
			int major, minor;
		};

		class GlfwApp {
		private:
			const GLVersion& version;
			Scene& scene;
			Logger logger;
			
		public:
			GlfwApp(Scene& s, const GLVersion& v = { 4, 5 })
				:version(v), scene(s) {
				logger = Logger::get("GlfwApp");
			}

			void recenter(GLFWwindow* window, Mouse& mouse) {
				if (mouse._recenter) {
					glfwSetCursorPos(window, center.x, center.y);
					mouse.pos = center;
					mouse.relativePos = glm::vec2(0);
					mouse._recenter = false;
				}
			}

			void run() {
//				using namespace ui;
				try {
					Keyboard::init();
					GLFWwindow* window;
					double currentTime = 0;
					glm::dvec2 pos;

					glfwSetErrorCallback(onError);

					if (!glfwInit()) {
						throw std::runtime_error("GLFW intialization failed");
					}

					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

					GLFWmonitor* monitor = scene.fullScreen() ? glfwGetPrimaryMonitor() : nullptr; // glfwGetPrimaryMonitor();

					window = glfwCreateWindow(scene.width(), scene.height(), scene.title(), monitor, nullptr);

					if (!window) {
						throw std::runtime_error("GLFW window creation failed");
					}

					glfwSetKeyCallback(window, onKey);
					glfwMakeContextCurrent(window);

					int loaded = ogl_LoadFunctions();
					if (loaded == ogl_LOAD_FAILED) {
						int failedCount = loaded - ogl_LOAD_SUCCEEDED;
						throw std::runtime_error("failed loading gl functions");
					}

					glfwSwapInterval(0);

#ifdef CONNECT_3D
					initSpacePro(window, scene.title());
#endif					

					Timer::start();
					Mouse::init();
					Font::init(scene.width(), scene.height());
					scene.init0();
#ifdef CONNECT_3D					
					register3DMotionEventHandler(scene.motionEventHandler());
#endif
					Mouse* mouse = nullptr;
					if (scene.requireMouse()) {
						glfwSetMouseButtonCallback(window, onMouseClick);
						glfwSetCursorPosCallback(window, onMouseMove);
						if (scene.hideCursor()) {
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						}
						mouse = &Mouse::get();
						mouse->recenter();
						center = scene.center();
						recenter(window, *mouse);
					}

					const std::vector<KeyListener>& listeners = scene.KeyListeners();
					keyListeners = std::vector<KeyListener>(listeners.begin(), listeners.end());

					auto sceneMouseClickListeners = scene.mouseClickListeners();
					mouseClickListners = std::vector<MouseClickListner>(sceneMouseClickListeners.begin(), sceneMouseClickListeners.end());

					auto sceneMouseMoveListners = scene.mouseMoveListners();
					mouseMoveListners = std::vector<MouseMoveListner>(sceneMouseMoveListners.begin(), sceneMouseMoveListners.end());

					if(!scene.vSync()) glfwSwapInterval(0);

					while (!glfwWindowShouldClose(window)) {
						int width, height;

						glfwGetFramebufferSize(window, &width, &height);
						Timer::get().update();

						
						glViewport(0, 0, width, height);
						scene.prepare();
						scene.resize(width, height);	// TODO resize only when screen changed
						scene.update0(Timer::get().lastFrameTime);
						scene.display0();

						glfwSwapBuffers(window);

						if (mouse) {
							recenter(window, *mouse);
						}
#ifdef CONNECT_3D
						handle3DConnexionEvents();
#endif
						glfwPollEvents();
					}

					Keyboard::dispose();
					glfwDestroyWindow(window);
					glfwTerminate();
				}
				catch (std::runtime_error& error) {
					glfwTerminate();
					std::string msg = "unable to start app:\t";
					logger.error(msg, error);
					std::cin.get();
				}
			}
		};

		int start(Scene* s, const GLVersion& v = { 4, 5 }) {
			GlfwApp app(*s, v);
			app.run();
			delete s;
			return 0;
		}
	}
}