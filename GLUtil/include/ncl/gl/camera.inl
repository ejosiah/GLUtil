#pragma once
#include <algorithm>
#include "common.h"

namespace ncl {
	namespace gl {
		const float Camera::DEFAULT_ROTATION_SPEED = 0.3f;
		const float Camera::DEFAULT_FOVX = 90.0f;
		const float Camera::DEFAULT_ZNEAR = 0.1f;
		const float Camera::DEFAULT_ZFAR = 1000.0f;

		const float Camera::DEFAULT_ORBIT_MIN_ZOOM = DEFAULT_ZNEAR + 1.0f;
		const float Camera::DEFAULT_ORBIT_MAX_ZOOM = DEFAULT_ZFAR * 0.5f;

		const float Camera::DEFAULT_ORBIT_OFFSET_DISTANCE = DEFAULT_ORBIT_MIN_ZOOM + (DEFAULT_ORBIT_MAX_ZOOM - DEFAULT_ORBIT_MIN_ZOOM) * 0.25f;

		const glm::vec3 Camera::WORLD_XAXIS(1.0f, 0.0f, 0.0f);
		const glm::vec3 Camera::WORLD_YAXIS(0.0f, 1.0f, 0.0f);
		const glm::vec3 Camera::WORLD_ZAXIS(0.0f, 0.0f, 1.0f);

		Camera::Camera()
		{
			using namespace glm;
			mode = FLIGHT;
			_preferTargetYAxisOrbiting = true;

			accumPitchDegrees = 0.0f;
			savedAccumPitchDegrees = 0.0f;

			rotationSpeed = DEFAULT_ROTATION_SPEED;
			fovx = DEFAULT_FOVX;
			aspectRatio = 0.0f;
			znear = DEFAULT_ZNEAR;
			zfar = DEFAULT_ZFAR;

			orbitMinZoom = DEFAULT_ORBIT_MIN_ZOOM;
			orbitMaxZoom = DEFAULT_ORBIT_MAX_ZOOM;
			orbitOffsetDistance = DEFAULT_ORBIT_OFFSET_DISTANCE;

			eye = vec3(0);
			savedEye = vec3(0);
			target = vec3(0);
			xAxis = vec3(1.0f, 0.0f, 0.0f);
			yAxis = vec3(0.0f, 1.0f, 0.0f);
			targetYAxis = vec3(0.0f, 1.0f, 0.0f);
			zAxis = vec3(0.0f, 0.0f, 1.0f);
			viewDir = vec3(0.0f, 0.0f, -1.0f);

			acceleration = vec3(0);
			currentVelocity = vec3(0);
			velocity = vec3(0);

			orientation = Orientation();
			savedOrientation = Orientation();

			viewMatrix = mat4(1);
			projMatrix = mat4(1);
			viewProjMatrix = mat4(1);
		}

		Camera::~Camera()
		{

		}

		void Camera::lookAt(const glm::vec3 &target)
		{
			lookAt(eye, target, yAxis);
		}

		void Camera::lookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up)
		{
			using namespace glm;
			this->eye = eye;
			this->target = target;

			viewMatrix = glm::lookAt(eye, target, up);
			// Extract the pitch angle from the view matrix.
			accumPitchDegrees = degrees(asinf(viewMatrix[1][2]));	// TODO change this matrix is colomn matrix

			xAxis = vec3(row(viewMatrix, 0));
			yAxis = vec3(row(viewMatrix, 1));
			zAxis = vec3(row(viewMatrix, 2));

			viewDir = -zAxis;

			accumPitchDegrees = degrees(asinf(viewMatrix[1][2]));

			orientation = Orientation(viewMatrix);
			updateViewMatrix();
		}

		void Camera::move(float dx, float dy, float dz)
		{
			// Moves the Camera by dx world units to the left or right; dy
			// world units upwards or downwards; and dz world units forwards
			// or backwards.

			if (mode == ORBIT)
			{
				// Orbiting Camera is always positioned relative to the
				// target position. See updateViewMatrix().
				return;
			}

			glm::vec3 eye = this->eye;
			glm::vec3 forwards;

			if (mode == FIRST_PERSON)
			{
				// Calculate the forwards direction. Can't just use the Camera's local
				// z axis as doing so will cause the Camera to move more slowly as the
				// Camera's view approaches 90 degrees straight up and down.

				forwards = cross(WORLD_YAXIS, xAxis);
				forwards = normalize(forwards);
			}
			else
			{
				forwards = viewDir;
			}

			eye += xAxis * dx;
			eye += WORLD_YAXIS * dy;
			eye += forwards * dz;

			setPosition(eye);
		}

		void Camera::move(const glm::vec3 &direction, const glm::vec3 &amount)
		{
			// Moves the Camera by the specified amount of world units in the specified
			// direction in world space.

			if (mode == ORBIT)
			{
				// Orbiting Camera is always positioned relative to the
				// target position. See updateViewMatrix().
				return;
			}

			eye.x += direction.x * amount.x;
			eye.y += direction.y * amount.y;
			eye.z += direction.z * amount.z;

			updateViewMatrix();
		}

		void Camera::perspective(float fovx, float aspect, float znear, float zfar)
		{
			projMatrix = glm::perspective(glm::radians(fovx), aspect, znear, zfar);

			viewProjMatrix = projMatrix * viewMatrix;

			this->fovx = fovx;
			aspectRatio = aspect;
			this->znear = znear;
			this->zfar = zfar;
		}

		void Camera::perspective(const Camera& otherCam) {
			perspective(otherCam.fovx, otherCam.aspectRatio, otherCam.znear, otherCam.zfar);
		}

		void Camera::rotate(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// Rotates the Camera based on its current mode.
			// Note that not all modes support rolling.

			pitchDegrees = -pitchDegrees;
			headingDegrees = -headingDegrees;
			rollDegrees = -rollDegrees;

			switch (mode)
			{
			default:
				break;

			case FIRST_PERSON:
			case SPECTATOR:
				rotateFirstPerson(headingDegrees, pitchDegrees);
				break;

			case FLIGHT:
				rotateFlight(headingDegrees, pitchDegrees, rollDegrees);
				break;

			case ORBIT:
				rotateOrbit(headingDegrees, pitchDegrees, rollDegrees);
				break;
			}

			updateViewMatrix();
		}

		void Camera::rotateSmoothly(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// This method applies a scaling factor to the rotation angles prior to
			// using these rotation angles to rotate the Camera. This method is usually
			// called when the Camera is being rotated using an input device (such as a
			// mouse or a joystick). 

			headingDegrees *= rotationSpeed;
			pitchDegrees *= rotationSpeed;
			rollDegrees *= rotationSpeed;

			rotate(headingDegrees, pitchDegrees, rollDegrees);
		}

		void Camera::undoRoll()
		{
			// Undo any Camera rolling by leveling the Camera. When the Camera is
			// orbiting this method will cause the Camera to become level with the
			// orbit target.

			if (mode == ORBIT)
				lookAt(eye, target, targetYAxis);
			else
				lookAt(eye, eye + viewDir, WORLD_YAXIS);
		}

		void Camera::updatePosition(const glm::vec3 &direction, float elapsedTimeSec)
		{
			// Moves the Camera using Newton's second law of motion. Unit mass is
			// assumed here to somewhat simplify the calculations. The direction vector
			// is in the range [-1,1].
			using namespace glm;
			if (dot(currentVelocity, currentVelocity) != 0.0f)
			{
				// Only move the Camera if the velocity vector is not of zero length.
				// Doing this guards against the Camera slowly creeping around due to
				// floating point rounding errors.

				glm::vec3 displacement = (currentVelocity * elapsedTimeSec) +
					(0.5f * acceleration * elapsedTimeSec * elapsedTimeSec);

				// Floating point rounding errors will slowly accumulate and cause the
				// Camera to move along each axis. To prevent any unintended movement
				// the displacement vector is clamped to zero for each direction that
				// the Camera isn't moving in. Note that the updateVelocity() method
				// will slowly decelerate the Camera's velocity back to a stationary
				// state when the Camera is no longer moving along that direction. To
				// account for this the Camera's current velocity is also checked.

				if (direction.x == 0.0f && closeEnough(currentVelocity.x, 0.0f))
					displacement.x = 0.0f;

				if (direction.y == 0.0f && closeEnough(currentVelocity.y, 0.0f))
					displacement.y = 0.0f;

				if (direction.z == 0.0f && closeEnough(currentVelocity.z, 0.0f))
					displacement.z = 0.0f;

				move(displacement.x, displacement.y, displacement.z);
			}

			// Continuously update the Camera's velocity vector even if the Camera
			// hasn't moved during this call. When the Camera is no longer being moved
			// the Camera is decelerating back to its stationary state.

			updateVelocity(direction, elapsedTimeSec);
		}

		void Camera::zoom(float zoom, float minZoom, float maxZoom)
		{
			if (mode == ORBIT)
			{
				// Moves the Camera closer to or further away from the orbit
				// target. The zoom amounts are in world units.

				orbitMaxZoom = maxZoom;
				orbitMinZoom = minZoom;

				glm::vec3 offset = eye - target;

				orbitOffsetDistance = glm::length(offset);
				offset = normalize(offset);
				orbitOffsetDistance += zoom;
				orbitOffsetDistance = std::min(std::max(orbitOffsetDistance, minZoom), maxZoom);

				offset *= orbitOffsetDistance;
				eye = offset + target;

				updateViewMatrix();
			}
			else
			{
				// For the other modes zoom is interpreted as changing the
				// horizontal field of view. The zoom amounts refer to the horizontal
				// field of view in degrees.

				zoom = std::min(std::max(zoom, minZoom), maxZoom);
				perspective(zoom, aspectRatio, znear, zfar);
			}
		}

		void Camera::setAcceleration(const glm::vec3 &acceleration)
		{
			this->acceleration = acceleration;
		}

		void Camera::setMode(Mode newMode)
		{
			// Switch to a new Camera mode (i.e., mode).
			// This method is complicated by the fact that it tries to save the current
			// mode's state prior to making the switch to the new Camera mode.
			// Doing this allows seamless switching between Camera modes.

			Mode prevMode = mode;

			if (prevMode == newMode)
				return;

			mode = newMode;

			switch (newMode)
			{
			case FIRST_PERSON:
				switch (prevMode)
				{
				default:
					break;

				case FLIGHT:
					eye.y = firstPersonYOffset;
					updateViewMatrix();
					break;

				case SPECTATOR:
					eye.y = firstPersonYOffset;
					updateViewMatrix();
					break;

				case ORBIT:
					eye.x = savedEye.x;
					eye.z = savedEye.z;
					eye.y = firstPersonYOffset;
					orientation = savedOrientation;
					accumPitchDegrees = savedAccumPitchDegrees;
					updateViewMatrix();
					break;
				}

				undoRoll();
				break;

			case SPECTATOR:
				switch (prevMode)
				{
				default:
					break;

				case FLIGHT:
					updateViewMatrix();
					break;

				case ORBIT:
					eye = savedEye;
					orientation = savedOrientation;
					accumPitchDegrees = savedAccumPitchDegrees;
					updateViewMatrix();
					break;
				}

				undoRoll();
				break;

			case FLIGHT:
				if (prevMode == ORBIT)
				{
					eye = savedEye;
					orientation = savedOrientation;
					accumPitchDegrees = savedAccumPitchDegrees;
					updateViewMatrix();
				}
				else
				{
					savedEye = eye;
					updateViewMatrix();
				}
				break;

			case ORBIT:
				if (prevMode == FIRST_PERSON)
					firstPersonYOffset = eye.y;

				savedEye = eye;
				savedOrientation = orientation;
				savedAccumPitchDegrees = accumPitchDegrees;

				targetYAxis = yAxis;

				glm::vec3 newEye = eye + zAxis * orbitOffsetDistance;
				glm::vec3 newTarget = eye;

				lookAt(newEye, newTarget, targetYAxis);
				break;
			}
		}

		void Camera::setCurrentVelocity(const glm::vec3 &currentVelocity)
		{
			this->currentVelocity = currentVelocity;
		}

		void Camera::setCurrentVelocity(float x, float y, float z)
		{
			this->currentVelocity.x = x;
			this->currentVelocity.y = y;
			this->currentVelocity.z = z;;
		}

		void Camera::setOrbitMaxZoom(float orbitMaxZoom)
		{
			this->orbitMaxZoom = orbitMaxZoom;
		}

		void Camera::setOrbitMinZoom(float orbitMinZoom)
		{
			this->orbitMinZoom = orbitMinZoom;
		}

		void Camera::setOrbitOffsetDistance(float orbitOffsetDistance)
		{
			this->orbitOffsetDistance = orbitOffsetDistance;
		}

		void Camera::setOrientation(const Orientation &newOrientation)
		{
			glm::mat4 m = glm::mat4_cast(newOrientation);

			// Store the pitch for this new orientation.
			// First person and spectator modes limit pitching to
			// 90 degrees straight up and down.

			accumPitchDegrees = glm::degrees(asinf(m[1][2]));

			// First person and spectator modes don't allow rolling.
			// Negate any rolling that might be encoded in the new orientation.

			orientation = newOrientation;

			if (mode == FIRST_PERSON || mode == SPECTATOR)
				lookAt(eye, eye + viewDir, WORLD_YAXIS);

			updateViewMatrix();
		}

		void Camera::setPosition(const glm::vec3 &newEye)
		{
			eye = newEye;
			updateViewMatrix();
		}

		void Camera::setPreferTargetYAxisOrbiting(bool preferTargetYAxisOrbiting)
		{
			// Determines the mode of Y axis rotations when the Camera is
			// orbiting a target. When preferTargetYAxisOrbiting is true all
			// Y axis rotations are about the orbit target's local Y axis.
			// When preferTargetYAxisOrbiting is false then the Camera's
			// local Y axis is used instead.

			_preferTargetYAxisOrbiting = preferTargetYAxisOrbiting;

			if (_preferTargetYAxisOrbiting)
				undoRoll();
		}

		void Camera::setRotationSpeed(float rotationSpeed)
		{
			// This is just an arbitrary value used to scale rotations
			// when rotateSmoothly() is called.

			this->rotationSpeed = rotationSpeed;
		}

		void Camera::setVelocity(const glm::vec3 &velocity)
		{
			this->velocity = velocity;
		}

		void Camera::setVelocity(float x, float y, float z)
		{
			velocity.x = x;
			velocity.y = y;
			velocity.z = z;
		}

		void Camera::rotateFirstPerson(float headingDegrees, float pitchDegrees)
		{
			// Implements the rotation logic for the first person style and
			// spectator style Camera modes. Roll is ignored.
			using namespace glm;
			accumPitchDegrees += pitchDegrees;

			if (accumPitchDegrees > 90.0f)
			{
				pitchDegrees = 90.0f - (accumPitchDegrees - pitchDegrees);
				accumPitchDegrees = 90.0f;
			}

			if (accumPitchDegrees < -90.0f)
			{
				pitchDegrees = -90.0f - (accumPitchDegrees - pitchDegrees);
				accumPitchDegrees = -90.0f;
			}

			Orientation rot;

			// Rotate Camera about the world y axis.
			// Note the order the Orientationernions are multiplied. That is important!
			if (headingDegrees != 0.0f)
			{
				rot = fromAxisAngle(WORLD_YAXIS, radians(headingDegrees)); // TODO 
				orientation = rot * orientation;
			}

			// Rotate Camera about its local x axis.
			// Note the order the Orientationernions are multiplied. That is important!
			if (pitchDegrees != 0.0f)
			{
				rot = fromAxisAngle(WORLD_XAXIS, radians(pitchDegrees));
				orientation = orientation * rot;
			}
		}

		void Camera::rotateFlight(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// Implements the rotation logic for the flight style Camera mode.
			using namespace glm;
			accumPitchDegrees += pitchDegrees;

			if (accumPitchDegrees > 360.0f)
				accumPitchDegrees -= 360.0f;

			if (accumPitchDegrees < -360.0f)
				accumPitchDegrees += 360.0f;

			Orientation rot = Orientation({ radians(pitchDegrees), radians(headingDegrees), radians(rollDegrees) });
			orientation *= rot;
		}

		void Camera::rotateOrbit(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// Implements the rotation logic for the orbit style Camera mode.
			// Roll is ignored for target Y axis orbiting.
			//
			// Briefly here's how this orbit Camera implementation works. Switching to
			// the orbit Camera mode via the setBehavior() method will set the
			// Camera's orientation to match the orbit target's orientation. Calls to
			// rotateOrbit() will rotate this orientation. To turn this into a third
			// person style view the updateViewMatrix() method will move the Camera
			// position back 'orbitOffsetDistance' world units along the Camera's
			// local z axis from the orbit target's world position.
			using namespace glm;
			Orientation rot;

			if (_preferTargetYAxisOrbiting)
			{
				if (headingDegrees != 0.0f)
				{
					rot = fromAxisAngle(targetYAxis, headingDegrees);
					orientation = rot * orientation;
				}

				if (pitchDegrees != 0.0f)
				{
					rot = fromAxisAngle(WORLD_XAXIS, pitchDegrees);
					orientation = orientation * rot;
				}
			}
			else
			{
				rot = Orientation({ radians(pitchDegrees), radians(headingDegrees), radians(rollDegrees) });
				orientation *= rot;
			}
		}

		void Camera::updateVelocity(const glm::vec3 &direction, float elapsedTimeSec)
		{
			// Updates the Camera's velocity based on the supplied movement direction
			// and the elapsed time (since this method was last called). The movement
			// direction is in the range [-1,1].

			if (direction.x != 0.0f)
			{
				// Camera is moving along the x axis.
				// Linearly accelerate up to the Camera's max speed.

				currentVelocity.x += direction.x * acceleration.x * elapsedTimeSec;

				if (currentVelocity.x > velocity.x)
					currentVelocity.x = velocity.x;
				else if (currentVelocity.x < -velocity.x)
					currentVelocity.x = -velocity.x;
			}
			else
			{
				// Camera is no longer moving along the x axis.
				// Linearly decelerate back to stationary state.

				if (currentVelocity.x > 0.0f)
				{
					if ((currentVelocity.x -= acceleration.x * elapsedTimeSec) < 0.0f)
						currentVelocity.x = 0.0f;
				}
				else
				{
					if ((currentVelocity.x += acceleration.x * elapsedTimeSec) > 0.0f)
						currentVelocity.x = 0.0f;
				}
			}

			if (direction.y != 0.0f)
			{
				// Camera is moving along the y axis.
				// Linearly accelerate up to the Camera's max speed.

				currentVelocity.y += direction.y * acceleration.y * elapsedTimeSec;

				if (currentVelocity.y > velocity.y)
					currentVelocity.y = velocity.y;
				else if (currentVelocity.y < -velocity.y)
					currentVelocity.y = -velocity.y;
			}
			else
			{
				// Camera is no longer moving along the y axis.
				// Linearly decelerate back to stationary state.

				if (currentVelocity.y > 0.0f)
				{
					if ((currentVelocity.y -= acceleration.y * elapsedTimeSec) < 0.0f)
						currentVelocity.y = 0.0f;
				}
				else
				{
					if ((currentVelocity.y += acceleration.y * elapsedTimeSec) > 0.0f)
						currentVelocity.y = 0.0f;
				}
			}

			if (direction.z != 0.0f)
			{
				// Camera is moving along the z axis.
				// Linearly accelerate up to the Camera's max speed.

				currentVelocity.z += direction.z * acceleration.z * elapsedTimeSec;

				if (currentVelocity.z > velocity.z)
					currentVelocity.z = velocity.z;
				else if (currentVelocity.z < -velocity.z)
					currentVelocity.z = -velocity.z;
			}
			else
			{
				// Camera is no longer moving along the z axis.
				// Linearly decelerate back to stationary state.

				if (currentVelocity.z > 0.0f)
				{
					if ((currentVelocity.z -= acceleration.z * elapsedTimeSec) < 0.0f)
						currentVelocity.z = 0.0f;
				}
				else
				{
					if ((currentVelocity.z += acceleration.z * elapsedTimeSec) > 0.0f)
						currentVelocity.z = 0.0f;
				}
			}
		}

		void Camera::updateViewMatrix()
		{
			// Reconstruct the view matrix.
			using namespace glm;
			viewMatrix = mat4_cast(orientation);

			xAxis = vec3(row(viewMatrix, 0));	// TODO verify that this is correct
			yAxis = vec3(row(viewMatrix, 1));
			zAxis = vec3(row(viewMatrix, 2));
			viewDir = -zAxis;

			if (mode == ORBIT)
			{
				// Calculate the new Camera position based on the current
				// orientation. The Camera must always maintain the same
				// distance from the target. Use the current offset vector
				// to determine the correct distance from the target.

				eye = target + zAxis * orbitOffsetDistance;
			}


			viewMatrix[3][0] = -dot(xAxis, eye);
			viewMatrix[3][1] = -dot(yAxis, eye);
			viewMatrix[3][2] =  -dot(zAxis, eye);
		}

		std::string Camera::modeAsString() const{
			switch (mode) {
			case ORBIT:
				return "Orbit";
			case FLIGHT:
				return "Flight";
			case FIRST_PERSON:
				return "First person";
			case SPECTATOR:
				return "Spectator";
			}
		}

	}
}