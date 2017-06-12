//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#if !defined(CAMERA_H)
#define CAMERA_H

#include <glm/glm.hpp>
#include "mathlib.h"

//-----------------------------------------------------------------------------
// A general purpose 6DoF (six degrees of freedom) quaternion based camera.
//
// This camera class supports 4 different behaviors:
// first person mode, spectator mode, flight mode, and orbit mode.
//
// First person mode only allows 5DOF (x axis movement, y axis movement, z axis
// movement, yaw, and pitch) and movement is always parallel to the world x-z
// (ground) plane.
//
// Spectator mode is similar to first person mode only movement is along the
// direction the camera is pointing.
// 
// Flight mode supports 6DoF. This is the camera class' default behavior.
//
// Orbit mode rotates the camera around a target position. This mode can be
// used to simulate a third person camera. Orbit mode supports 2 modes of
// operation: orbiting about the target's Y axis, and free orbiting. The former
// mode only allows pitch and yaw. All yaw changes are relative to the target's
// local Y axis. This means that camera yaw changes won't be affected by any
// rolling. The latter mode allows the camera to freely orbit the target. The
// camera is free to pitch, yaw, and roll. All yaw changes are relative to the
// camera's orientation (in space orbiting the target).
//
// This camera class allows the camera to be moved in 2 ways: using fixed
// step world units, and using a supplied velocity and acceleration. The former
// simply moves the camera by the specified amount. To move the camera in this
// way call one of the move() methods. The other way to move the camera
// calculates the camera's displacement based on the supplied velocity,
// acceleration, and elapsed time. To move the camera in this way call the
// updatePosition() method.
//-----------------------------------------------------------------------------
namespace ncl {
	namespace gl {
		class Camera
		{
			friend class CameraController;
		public:
			enum Mode
			{
				FIRST_PERSON,
				SPECTATOR,
				FLIGHT,
				ORBIT
			};

			Camera();
			~Camera();

			void lookAt(const Vector3 &target);
			void lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up);
			void move(float dx, float dy, float dz);
			void move(const Vector3 &direction, const Vector3 &amount);
			void perspective(float fovx, float aspect, float znear, float zfar);
			void perspective(const Camera& otherCam);
			void rotate(float headingDegrees, float pitchDegrees, float rollDegrees);
			void rotateSmoothly(float headingDegrees, float pitchDegrees, float rollDegrees);
			void undoRoll();
			void updatePosition(const Vector3 &direction, float elapsedTimeSec);
			void zoom(float zoom, float minZoom, float maxZoom);

			// Getter methods.

			const Vector3 &getAcceleration() const;
			Mode getMode() const;
			const Vector3 &getCurrentVelocity() const;
			const Vector3 &getPosition() const;
			float getOrbitMinZoom() const;
			float getOrbitMaxZoom() const;
			float getOrbitOffsetDistance() const;
			float getOrbitPitchMaxDegrees() const;
			float getOrbitPitchMinDegrees() const;
			const Quaternion &getOrientation() const;
			float getRotationSpeed() const;
			const Matrix4 &getProjectionMatrix() const;
			const Vector3 &getVelocity() const;
			const Vector3 &getViewDirection() const;
			const Matrix4 &getViewMatrix() const;
			const Matrix4 &getViewProjectionMatrix() const;
			const Vector3 &getXAxis() const;
			const Vector3 &getYAxis() const;
			const Vector3 &getZAxis() const;
			bool preferTargetYAxisOrbiting() const;
			const glm::mat4 view() const;
			const glm::mat4 projection() const;

			// Setter methods.

			void setAcceleration(const Vector3 &acceleration);
			void setMode(Mode newBehavior);
			void setCurrentVelocity(const Vector3 &currentVelocity);
			void setCurrentVelocity(float x, float y, float z);
			void setOrbitMaxZoom(float orbitMaxZoom);
			void setOrbitMinZoom(float orbitMinZoom);
			void setOrbitOffsetDistance(float orbitOffsetDistance);
			void setOrbitPitchMaxDegrees(float orbitPitchMaxDegrees);
			void setOrbitPitchMinDegrees(float orbitPitchMinDegrees);
			void setOrientation(const Quaternion &newOrientation);
			void setPosition(const Vector3 &newEye);
			void setPreferTargetYAxisOrbiting(bool preferTargetYAxisOrbiting);
			void setRotationSpeed(float rotationSpeed);
			void setVelocity(const Vector3 &velocity);
			void setVelocity(float x, float y, float z);

		private:
			void rotateFirstPerson(float headingDegrees, float pitchDegrees);
			void rotateFlight(float headingDegrees, float pitchDegrees, float rollDegrees);
			void rotateOrbit(float headingDegrees, float pitchDegrees, float rollDegrees);
			void updateVelocity(const Vector3 &direction, float elapsedTimeSec);
			void updateViewMatrix();

			static const float DEFAULT_ROTATION_SPEED;
			static const float DEFAULT_FOVX;
			static const float DEFAULT_ZNEAR;
			static const float DEFAULT_ZFAR;
			static const float DEFAULT_ORBIT_MIN_ZOOM;
			static const float DEFAULT_ORBIT_MAX_ZOOM;
			static const float DEFAULT_ORBIT_OFFSET_DISTANCE;
			static const Vector3 WORLD_XAXIS;
			static const Vector3 WORLD_YAXIS;
			static const Vector3 WORLD_ZAXIS;

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
			Vector3 eye;
			Vector3 savedEye;
			Vector3 target;
			Vector3 targetYAxis;
			Vector3 xAxis;
			Vector3 yAxis;
			Vector3 zAxis;
			Vector3 viewDir;
			Vector3 acceleration;
			Vector3 currentVelocity;
			Vector3 velocity;
			Quaternion orientation;
			Quaternion savedOrientation;
			Matrix4 viewMatrix;
			Matrix4 projMatrix;
			Matrix4 viewProjMatrix;
		};

		//-----------------------------------------------------------------------------

		inline const Vector3 &Camera::getAcceleration() const
		{
			return acceleration;
		}

		inline Camera::Mode Camera::getMode() const
		{
			return mode;
		}

		inline const Vector3 &Camera::getCurrentVelocity() const
		{
			return currentVelocity;
		}

		inline const Vector3 &Camera::getPosition() const
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

		inline const Quaternion &Camera::getOrientation() const
		{
			return orientation;
		}

		inline float Camera::getRotationSpeed() const
		{
			return rotationSpeed;
		}

		inline const Matrix4 &Camera::getProjectionMatrix() const
		{
			return projMatrix;
		}

		inline const Vector3 &Camera::getVelocity() const
		{
			return velocity;
		}

		inline const Vector3 &Camera::getViewDirection() const
		{
			return viewDir;
		}

		inline const Matrix4 &Camera::getViewMatrix() const
		{
			return viewMatrix;
		}

		inline const Matrix4 &Camera::getViewProjectionMatrix() const
		{
			return viewProjMatrix;
		}

		inline const Vector3 &Camera::getXAxis() const
		{
			return xAxis;
		}

		inline const Vector3 &Camera::getYAxis() const
		{
			return yAxis;
		}

		inline const Vector3 &Camera::getZAxis() const
		{
			return zAxis;
		}

		inline bool Camera::preferTargetYAxisOrbiting() const
		{
			return _preferTargetYAxisOrbiting;
		}
	}
}

#include "camera.inl"
#endif