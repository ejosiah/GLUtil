#pragma once
#pragma once

#include <string>
#include <stdexcept>
#include <Windows.h>
#include <glm/glm.hpp>
#include <functional>

/* SpaceWare Specific Includes */
#include "spwmacro.h"  /* Common macros used by SpaceWare functions. */
#include "si.h"        /* Required for any SpaceWare support within an app.*/
#include "siapp.h"     /* Required for siapp.lib symbols */
#include <GLFW\glfw3.h>

#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW\glfw3native.h>

#include "logger.h"
#include "common.h"

#include "_virtualkeys.hpp"

#pragma comment(lib, "siapp.lib")
#pragma comment(lib, "spwmath.lib")
#pragma comment(lib, "spwmathD.lib")
#pragma comment(lib, "spwmathMT.lib")
#pragma comment(lib, "spwmathMTD.lib")

namespace ncl {
	namespace gl {

		static SiHdl devHdl;
		static SiSpwEvent spEvent;    /* SpaceWare Event */
		static SiGetEventData EData;    /* SpaceWare Event Data */
		static MSG msg;
		static BOOL handled;
		static float rotationSensitivity;
		static float translationSensitivity;

		static _3DMotionEventHandler* motionEventHandler;

		void initSpacePro(GLFWwindow* window, const char* appName, float rSensitivity = 1.0f, float tSensitivity = 1.0f) {
			ncl::Logger logger = ncl::Logger::get("3DConnexion");

			HWND hWndMain = glfwGetWin32Window(window);
			SiOpenData oData;

			if (SiInitialize() == SPW_DLL_LOAD_ERROR) {
				logger.warn("Could not load siAppDll dll files");
				return;
			}

			SiOpenWinInit(&oData, hWndMain);

			if ((devHdl = SiOpen(appName, SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, &oData)) == NULL) {
				SiTerminate();
				logger.warn("unable to connect to SpaceMouse pro");
			}
			else {
				SiDeviceName devName;
				SiGetDeviceName(devHdl, &devName);
				rotationSensitivity = rSensitivity;
				translationSensitivity = tSensitivity;
				logger.info(std::string(devName.name) + " connected");
				
			}

		}

		void register3DMotionEventHandler(_3DMotionEventHandler*  handler) {
			if (handler != nullptr) {
				motionEventHandler = handler;
			}
		}

		void handle3DConnexionEvents() {
			handled = SPW_FALSE;
			if (devHdl) {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

					SiGetEventWinInit(&EData, msg.message, msg.wParam, msg.lParam);

					if (SiGetEvent(devHdl, SI_AVERAGE_EVENTS, &EData, &spEvent) == SI_IS_EVENT) {
						_3DMotionEvent event;
						switch (spEvent.type) {
						case SI_MOTION_EVENT:
							event.translation.x = spEvent.u.spwData.mData[SI_TX];
							event.translation.y = spEvent.u.spwData.mData[SI_TY];
							event.translation.z = spEvent.u.spwOrientation == SiOrientation::SI_LEFT ? -spEvent.u.spwData.mData[SI_TZ] : spEvent.u.spwData.mData[SI_TZ];

							event.rotation.x = glm::clamp(float(spEvent.u.spwData.mData[SI_RX]), -360.0f, 360.0f);
							event.rotation.y = glm::clamp(float(spEvent.u.spwData.mData[SI_RY]), -360.0f, 360.0f);
							event.rotation.z = glm::clamp(float(spEvent.u.spwOrientation == SiOrientation::SI_LEFT ? -spEvent.u.spwData.mData[SI_RZ] : spEvent.u.spwData.mData[SI_RZ]), -360.0f, 360.0f);

							event.rotation *= rotationSensitivity;
							event.translation *= translationSensitivity;

							motionEventHandler->onMotion(event);

							clear(event.translation);
							clear(event.rotation);

							break;
						case SI_ZERO_EVENT:
							motionEventHandler->onNoMotion();
							break;
						}
						handled = SPW_TRUE;
					}

					if (handled == SPW_FALSE) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}
		}

	}
}