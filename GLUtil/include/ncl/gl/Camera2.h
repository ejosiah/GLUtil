#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ncl {
	namespace gl {
		class Camera2
		{
			friend class Camera2Controller;
		public:
			enum Mode
			{
				FIRST_PERSON,
				SPECTATOR,
				FLIGHT,
				ORBIT
			};

			Camera2();
			~Camera2();

			void lookAt(const glm::vec3 &target);
			void lookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up);
			void move(float dx, float dy, float dz);
			void move(const glm::vec3 &direction, const glm::vec3 &amount);
			void perspective(float fovx, float aspect, float znear, float zfar);
			void perspective(const Camera2& otherCam);
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
			const glm::quat &getOrientation() const;
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
			const glm::mat4 view() const;
			const glm::mat4 projection() const;

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
			void setOrientation(const glm::quat &newOrientation);
			void setPosition(const glm::vec3 &newEye);
			void setPreferTargetYAxisOrbiting(bool preferTargetYAxisOrbiting);
			void setRotationSpeed(float rotationSpeed);
			void setVelocity(const glm::vec3 &velocity);
			void setVelocity(float x, float y, float z);

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
			glm::quat orientation;
			glm::quat savedOrientation;
			glm::mat4 viewMatrix;
			glm::mat4 projMatrix;
			glm::mat4 viewProjMatrix;
		};

		//-----------------------------------------------------------------------------

		inline const glm::vec3 &Camera2::getAcceleration() const
		{
			return acceleration;
		}

		inline Camera2::Mode Camera2::getMode() const
		{
			return mode;
		}

		inline const glm::vec3 &Camera2::getCurrentVelocity() const
		{
			return currentVelocity;
		}

		inline const glm::vec3 &Camera2::getPosition() const
		{
			return eye;
		}

		inline float Camera2::getOrbitMinZoom() const
		{
			return orbitMinZoom;
		}

		inline float Camera2::getOrbitMaxZoom() const
		{
			return orbitMaxZoom;
		}

		inline float Camera2::getOrbitOffsetDistance() const
		{
			return orbitOffsetDistance;
		}

		inline const glm::quat &Camera2::getOrientation() const
		{
			return orientation;
		}

		inline float Camera2::getRotationSpeed() const
		{
			return rotationSpeed;
		}

		inline const glm::mat4 &Camera2::getProjectionMatrix() const
		{
			return projMatrix;
		}

		inline const glm::vec3 &Camera2::getVelocity() const
		{
			return velocity;
		}

		inline const glm::vec3 &Camera2::getViewDirection() const
		{
			return viewDir;
		}

		inline const glm::mat4 &Camera2::getViewMatrix() const
		{
			return viewMatrix;
		}

		inline const glm::mat4 &Camera2::getViewProjectionMatrix() const
		{
			return viewProjMatrix;
		}

		inline const glm::vec3 &Camera2::getXAxis() const
		{
			return xAxis;
		}

		inline const glm::vec3 &Camera2::getYAxis() const
		{
			return yAxis;
		}

		inline const glm::vec3 &Camera2::getZAxis() const
		{
			return zAxis;
		}

		inline bool Camera2::preferTargetYAxisOrbiting() const
		{
			return _preferTargetYAxisOrbiting;
		}
	}
}

#include "Camera2.inl"