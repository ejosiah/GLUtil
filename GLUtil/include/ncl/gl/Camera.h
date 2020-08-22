#pragma once
#include <glm/glm.hpp>
#include "orientation.h"
#include "Shader.h"

namespace ncl {
	namespace gl {
		class Camera
		{
		public:
			friend class CameraController;
			enum Mode
			{
				FIRST_PERSON,
				SPECTATOR,
				FLIGHT,
				ORBIT
			};

			Camera();
			~Camera();

			void lookAt(const glm::vec3 &target);
			void lookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up);
			void move(float dx, float dy, float dz);
			void move(const glm::vec3 &direction, const glm::vec3 &amount);
			void perspective(float fovx, float aspect, float znear, float zfar);
			void perspective(float aspect);
			void perspective(const Camera& otherCam);
			void rotate(float headingDegrees, float pitchDegrees, float rollDegrees);
			void rotateSmoothly(float headingDegrees, float pitchDegrees, float rollDegrees);
			void undoRoll();
			void updatePosition(const glm::vec3 &direction, float elapsedTimeSec);
			void zoom(float zoom, float minZoom, float maxZoom);

			// Getter methods.

			const glm::vec3 &getAcceleration() const;
			Mode getMode() const;
			const glm::vec3 &getCurrentVelocity() const;
			const glm::vec3 &getPosition() const;
			float getOrbitMinZoom() const;
			float getOrbitMaxZoom() const;
			float getOrbitOffsetDistance() const;
			float getOrbitPitchMaxDegrees() const;
			float getOrbitPitchMinDegrees() const;
			const Orientation &getOrientation() const;
			float getRotationSpeed() const;
			const glm::mat4 &getProjectionMatrix() const;
			const glm::vec3 &getVelocity() const;
			const glm::vec3 &getViewDirection() const;
			const glm::mat4 &getViewMatrix() const;
			const glm::mat4 &getViewProjectionMatrix() const;
			const glm::vec3 &getXAxis() const;
			const glm::vec3 &getYAxis() const;
			const glm::vec3 &getZAxis() const;
			bool preferTargetYAxisOrbiting() const;

			// Setter methods.

			void setAcceleration(const glm::vec3 &acceleration);
			void setMode(Mode newBehavior);
			void setCurrentVelocity(const glm::vec3 &currentVelocity);
			void setCurrentVelocity(float x, float y, float z);
			void setOrbitMaxZoom(float orbitMaxZoom);
			void setOrbitMinZoom(float orbitMinZoom);
			void setOrbitOffsetDistance(float orbitOffsetDistance);
			void setOrbitPitchMaxDegrees(float orbitPitchMaxDegrees);
			void setOrbitPitchMinDegrees(float orbitPitchMinDegrees);
			void setOrientation(const Orientation &newOrientation);
			void setPosition(const glm::vec3 &newEye);
			void setPreferTargetYAxisOrbiting(bool preferTargetYAxisOrbiting);
			void setRotationSpeed(float rotationSpeed);
			void setVelocity(const glm::vec3 &velocity);
			void setVelocity(float x, float y, float z);
			std::string modeAsString() const;

			inline void collisionTestOn() {
				testCollision = true;
			}

			inline void collisionTestOff() {
				testCollision = false;
			}

			inline float getNear() {
				return znear;
			}

			inline float getFar() {
				return zfar;
			}

		private:
			void rotateFirstPerson(float headingDegrees, float pitchDegrees);
			void rotateFlight(float headingDegrees, float pitchDegrees, float rollDegrees);
			void rotateOrbit(float headingDegrees, float pitchDegrees, float rollDegrees);
			void updateVelocity(const glm::vec3 &direction, float elapsedTimeSec);
			void updateViewMatrix();
			

			static const float DEFAULT_ROTATION_SPEED;
			static const float DEFAULT_FOVX;
			static const float DEFAULT_ZNEAR;
			static const float DEFAULT_ZFAR;
			static const float DEFAULT_ORBIT_MIN_ZOOM;
			static const float DEFAULT_ORBIT_MAX_ZOOM;
			static const float DEFAULT_ORBIT_OFFSET_DISTANCE;
			static const glm::vec3 WORLD_XAXIS;
			static const glm::vec3 WORLD_YAXIS;
			static const glm::vec3 WORLD_ZAXIS;

			Mode mode;
			bool _preferTargetYAxisOrbiting;
			float accumPitchDegrees;
			float savedAccumPitchDegrees;
			float rotationSpeed;
			float fovx;
			float aspectRatio;
			float znear;
			float zfar;
			float orbitMinZoom;
			float orbitMaxZoom;
			float orbitOffsetDistance;
			float firstPersonYOffset;
			glm::vec3 eye;
			glm::vec3 savedEye;
			glm::vec3 target;
			glm::vec3 targetYAxis;
			glm::vec3 xAxis;
			glm::vec3 yAxis;
			glm::vec3 zAxis;
			glm::vec3 viewDir;
			glm::vec3 acceleration;
			glm::vec3 currentVelocity;
			glm::vec3 velocity;
			Orientation orientation;
			Orientation savedOrientation;
			glm::mat4 viewMatrix;
			glm::mat4 projMatrix;
			glm::mat4 viewProjMatrix;
			bool testCollision = true;
		};

		//-----------------------------------------------------------------------------

		inline const glm::vec3 &Camera::getAcceleration() const
		{
			return acceleration;
		}

		inline Camera::Mode Camera::getMode() const
		{
			return mode;
		}

		inline const glm::vec3 &Camera::getCurrentVelocity() const
		{
			return currentVelocity;
		}

		inline const glm::vec3 &Camera::getPosition() const
		{
			return eye;
		}

		inline float Camera::getOrbitMinZoom() const
		{
			return orbitMinZoom;
		}

		inline float Camera::getOrbitMaxZoom() const
		{
			return orbitMaxZoom;
		}

		inline float Camera::getOrbitOffsetDistance() const
		{
			return orbitOffsetDistance;
		}

		inline const Orientation &Camera::getOrientation() const
		{
			return orientation;
		}

		inline float Camera::getRotationSpeed() const
		{
			return rotationSpeed;
		}

		inline const glm::mat4 &Camera::getProjectionMatrix() const
		{
			return projMatrix;
		}

		inline const glm::vec3 &Camera::getVelocity() const
		{
			return velocity;
		}

		inline const glm::vec3 &Camera::getViewDirection() const
		{
			return viewDir;
		}

		inline const glm::mat4 &Camera::getViewMatrix() const
		{
			return viewMatrix;
		}

		inline const glm::mat4 &Camera::getViewProjectionMatrix() const
		{
			return viewProjMatrix;
		}

		inline const glm::vec3 &Camera::getXAxis() const
		{
			return xAxis;
		}

		inline const glm::vec3 &Camera::getYAxis() const
		{
			return yAxis;
		}

		inline const glm::vec3 &Camera::getZAxis() const
		{
			return zAxis;
		}

		inline bool Camera::preferTargetYAxisOrbiting() const
		{
			return _preferTargetYAxisOrbiting;
		}

		inline void Camera::perspective(float aspectRatio) {
			perspective(fovx, aspectRatio, znear, zfar);
		}

		/*
		class CameraController {

			virtual void init() = 0;

			virtual void update(float elapsedTime) = 0;

			virtual void updateAspectRation(float ratio) = 0;

			virtual void sendTo(Shader& shader) = 0;
		};*/
	}
}

#include "Camera.inl"