#pragma once

#include "Camera.h"
#include "input.h"
#include "mesh.h"
#include "Model.h"
#include <cstdlib>
#include "../geom/Plane.h"

namespace ncl {
	namespace gl {

		static const float     FLOOR_WIDTH = 8.0f;
		static const float     FLOOR_HEIGHT = 8.0f;


		static const float     CAMERA_FOVX = 60.0f;
		static const float     CAMERA_ZFAR = 1000.0f;
		static const float     CAMERA_ZNEAR = 0.1f;
		static const float     CAMERA_ZOOM_MAX = 5.0f;
		static const float     CAMERA_ZOOM_MIN = 1.5f;

		static const float     CAMERA_SPEED_FLIGHT_YAW = 100.0f;
		static const float     CAMERA_SPEED_ORBIT_ROLL = 100.0f;

		static const glm::vec3   CAMERA_ACCELERATION(4.0f, 4.0f, 4.0f);
		static const glm::vec3   CAMERA_VELOCITY(1.0f);

		struct CameraSettings {
			Mesurements sceneDimentions = {};
			Camera::Mode mode = Camera::SPECTATOR;
			float modelHeight = 1;
			Mesurements floor = { FLOOR_WIDTH , FLOOR_HEIGHT };
			float fovx = CAMERA_FOVX;
			float zNear = CAMERA_ZNEAR;
			float zFar = CAMERA_ZFAR;
			float maxZoom = CAMERA_ZOOM_MAX;
			float minZoom = CAMERA_ZOOM_MIN;
			float flightYawSpeed = CAMERA_SPEED_FLIGHT_YAW;
			float orbitRollSpeed = CAMERA_SPEED_ORBIT_ROLL;
			glm::vec3 acceleration = CAMERA_ACCELERATION;
			glm::vec3 velocty = CAMERA_VELOCITY;
		};

		class CameraController {
		private:
			Camera camera;
			float floorWidth;
			float floorHeight;
			float cameraFloorOffset;
			geom::Plane ground;
			float fovx;
			float zNear;
			float zFar;
			float maxZoom;
			float minZoom;
			float flightYawSpeed;
			float orbitRollSpeed;
			float modelHeight = 1;
			glm::vec3 acceleration;
			glm::vec3 velocty;
			Camera::Mode mode;
			Mesurements sceneDimentions;
			Mesurements floor;
			struct {
				glm::vec3 position;
				glm::quat orientation;
			} model;
			glm::vec3 direction;
			struct {
				glm::vec3 min;
				glm::vec3 max;
			} bounds;

		public:
			CameraController(const CameraSettings settings) :
				CameraController(
					settings.sceneDimentions,
					settings.mode,
					settings.modelHeight,
					settings.floor,
					settings.fovx,
					settings.zNear,
					settings.zFar,
					settings.maxZoom,
					settings.minZoom,
					settings.flightYawSpeed,
					settings.orbitRollSpeed,
					settings.acceleration,
					settings.velocty
				) {

			}

			CameraController(
				Mesurements sceneDimentions = {},
				Camera::Mode mode = Camera::SPECTATOR,
				float modelHeight = 1,
				Mesurements floor = { FLOOR_WIDTH , FLOOR_HEIGHT },
				float fovx = CAMERA_FOVX,
				float zNear = CAMERA_ZNEAR,
				float zFar = CAMERA_ZFAR,
				float maxZoom = CAMERA_ZOOM_MAX,
				float minZoom = CAMERA_ZOOM_MIN,
				float flightYawSpeed = CAMERA_SPEED_FLIGHT_YAW,
				float orbitRollSpeed = CAMERA_SPEED_ORBIT_ROLL,
				glm::vec3 acceleration = CAMERA_ACCELERATION,
				glm::vec3 velocty = CAMERA_VELOCITY
				) {

				this->floorWidth = floorWidth;
				this->floorHeight = floorHeight;
				this->fovx = fovx;
				this->zNear = zNear;
				this->zFar = zFar;
				this->maxZoom = maxZoom;
				this->minZoom = minZoom;
				this->flightYawSpeed = flightYawSpeed;
				this->orbitRollSpeed = orbitRollSpeed;
				this->modelHeight = modelHeight;
				this->acceleration = acceleration;
				this->velocty = velocty;
				this->mode = mode;
				this->sceneDimentions = sceneDimentions;
				this->direction = glm::vec3(0);
				this->floor = floor;
				this->ground = geom::Plane({ 0, 1, 0 }, 0);
				this->cameraFloorOffset = 1;

			}

			virtual void init() {
				using namespace std;
				float aspectRatio = sceneDimentions.width / sceneDimentions.height;
				camera.perspective(fovx, aspectRatio, zNear, zFar);

				
					
				camera.setPosition(glm::vec3(0.0f, cameraFloorOffset, 0.0f));
				camera.setOrbitMinZoom(minZoom);
				camera.setOrbitMaxZoom(maxZoom);
				camera.setOrbitOffsetDistance(minZoom);

				model.position = camera.getPosition();
				model.orientation =  inverse(camera.getOrientation());

				camera.setAcceleration(acceleration);
				camera.setVelocity(velocty);
				updateMode(mode);

				bounds.max = { floor.width / 2.0f, 1000.0f, floor.length / 2.0f };
				bounds.min = { -floor.width / 2.0f, 0, -floor.length / 2.0f };
			}

			void setModelHeight(float h) {
				modelHeight = h;
				cameraFloorOffset = modelHeight * 0.5f;
			}

			void setFloorMeasurement(Mesurements floor) {
				this->floor = floor;
			}

			void updateMode(Camera::Mode mode) {
				if (camera.getMode() == mode) return;

				if (mode == Camera::ORBIT) {
					model.position = camera.getPosition();
					model.orientation = inverse(camera.getOrientation());
				}

				camera.setMode(mode);
				if (mode == Camera::ORBIT) {
					camera.rotate(0.0f, -30.0f, 0.0f);
				}
			}

			const glm::mat4& project() const {
				return camera.getProjectionMatrix();
			}

			const glm::mat4& view() const {
				return camera.getViewMatrix();
			}

			const glm::mat4 modelTransform() const {
				glm::mat4 m = glm::mat4_cast(model.orientation);
				glm::mat4 v = camera.getViewMatrix();
				glm::mat4 p = camera.getProjectionMatrix();
				m = translate(m, model.position);

				return p * v * m;
			}

			const glm::mat4 modelTrans() const {
				glm::mat4 m = glm::mat4_cast(model.orientation);
				m = translate(m, model.position);

				return m;
			}

			virtual void update(float elapsedTime)  {
			
				Mouse& mouse = Mouse::get();
				float dx;
				float dy;
				float dz;

				switch (camera.getMode()) {
				case Camera::FIRST_PERSON:
				case Camera::SPECTATOR:
					dx = -mouse.relativePos.x;
					dy = -mouse.relativePos.y;

					camera.rotateSmoothly(dx, dy, 0.0f);
					camera.updatePosition(direction, elapsedTime);
					break;
				case Camera::FLIGHT:
					dy = mouse.relativePos.y;
					dz = -mouse.relativePos.x;
					camera.rotateSmoothly(0.0f, dy, dz);

					if ((dx = -direction.x * flightYawSpeed * elapsedTime) != 0.0f) {
						camera.rotate(dx, 0.0f, 0.0f);
					}

					direction.x = 0.0f; // ignore yaw motion when updating camera's velocity;
					camera.updatePosition(direction, elapsedTime);
					break;
				case Camera::ORBIT:
					using namespace std;
					dx = mouse.relativePos.x;
					dy = mouse.relativePos.y;

					camera.rotateSmoothly(dx, dy, 0.0f);

					if (!camera.preferTargetYAxisOrbiting()) {
						if ((dz = direction.x * orbitRollSpeed * elapsedTime) != 0.0f) {
							camera.rotate(0.0f, 0.0f, dz);
						}
					}

					if ((dz = -mouse.wheelPos) != 0.0f) {
						camera.zoom(dz, camera.getOrbitMinZoom(), camera.getOrbitMaxZoom());
					}

					break;
				default:
					break;
				}

				mouse.recenter();

				if (mouse.middle.status == Mouse::Button::PRESSED) {
					camera.undoRoll();
				}

				if (camera.testCollision) {
					performCollisionDetection();
				}
			}

			void performCollisionDetection() {	
			// TODO use proper collision detection
			// Very simple collision detection to prevent the first person, spectator
			// and flight cameras from moving too far outside of the scene. We don't
			// worry about the orbiting camera here because the min and max zoom							
			// constants already limits how far the orbiting camera can zoom in/out								
			// of the scene.

				if (camera.getMode() != Camera::ORBIT) {
					const glm::vec3 &pos = camera.getPosition();
					glm::vec3 newPos(pos);

					if (pos.x > bounds.max.x)
						newPos.x = bounds.max.x;

					if (pos.x < bounds.min.x)
						newPos.x = bounds.min.x;

					if (pos.y > bounds.max.y)
						newPos.y = bounds.max.y;

					if (pos.y < bounds.min.y)
						newPos.y = bounds.min.y;

					if (pos.z > bounds.max.z)
						newPos.z = bounds.max.z;

					if (pos.z < bounds.min.z)
						newPos.z = bounds.min.z;

					float depth = glm::dot(ground.n, pos);
					if (depth < (ground.d + cameraFloorOffset)) {
						newPos.y = cameraFloorOffset;
					}

					camera.setPosition(newPos);
				}
			}

			void processUserInput() {
				if (Keyboard::get().SPACE_BAR.pressed()) {
					camera.setPreferTargetYAxisOrbiting(!camera.preferTargetYAxisOrbiting());
				}
				if (Keyboard::get().NUM_1.pressed()) {
					updateMode(Camera::FIRST_PERSON);
				}
				if (Keyboard::get().NUM_2.pressed()) {
					updateMode(Camera::SPECTATOR);
				}
				if (Keyboard::get().NUM_3.pressed()) {
					updateMode(Camera::FLIGHT);
				}
				if (Keyboard::get().NUM_4.pressed()) {
					updateMode(Camera::ORBIT);
				}

				movementDirection(direction);
			}

			void movementDirection(glm::vec3& direction) {
				static bool moveForwardsPressed = false;
				static bool moveBackwardsPressed = false;
				static bool moveRightPressed = false;
				static bool moveLeftPressed = false;
				static bool moveUpPressed = false;
				static bool moveDownPressed = false;

				glm::vec3 velocty = camera.getCurrentVelocity();
				direction = glm::vec3(0.0f, 0.0f, 0.0f);

				if (Keyboard::get().W.pressed()) {
					if (!moveForwardsPressed) {
						moveForwardsPressed = true;
						camera.setCurrentVelocity(velocty.x, velocty.y, 0.0f);
					}
					direction.z += 1.0f;
				}
				else {
					moveForwardsPressed = false;
				}

				if (Keyboard::get().S.pressed()) {
					if (!moveBackwardsPressed) {
						moveBackwardsPressed = true;
						camera.setCurrentVelocity(velocty.x, velocty.y, 0.0f);
					}
					direction.z -= 1.0f;
				}
				else {
					moveBackwardsPressed = false;
				}

				if (Keyboard::get().D.pressed()) {
					if (!moveRightPressed) {
						moveRightPressed = true;
						camera.setCurrentVelocity(0.0f, velocty.y, velocty.z);
					}
					direction.x += 1.0f;
				}
				else {
					moveRightPressed = false;
				}

				if (Keyboard::get().A.pressed()) {
					if (!moveLeftPressed) {
						moveLeftPressed = true;
						camera.setCurrentVelocity(0.0f, velocty.y, velocty.z);
					}
					direction.x -= 1.0f;
				}
				else {
					moveLeftPressed = false;
				}

				if (Keyboard::get().E.pressed()) {
					if (!moveUpPressed) {
						moveUpPressed = true;
						camera.setCurrentVelocity(velocty.x, 0.0f, velocty.z);
					}
					direction.y += 1.0f;
				}
				else {
					moveUpPressed = false;
				}

				if (Keyboard::get().Q.pressed()) {
					if (!moveDownPressed) {
						moveDownPressed = true;
						camera.setCurrentVelocity(velocty.x, 0.0f, velocty.z);
					}
					direction.y -= 1.0f;
				}
				else {
					moveDownPressed = false;
				}
			}

			virtual void updateAspectRation(float ratio)  {
				camera.perspective(ratio);
			}

			Camera& getCamera() {
				return camera;
			}

			const Camera& getCamera() const {
				return camera;
			}

			inline float getNear() {
				return zNear;
			}

			inline float getFar() {
				return zFar;
			}
		};
	}
}