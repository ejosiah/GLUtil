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

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace ncl {
	namespace gl {
		const float Camera::DEFAULT_ROTATION_SPEED = 0.3f;
		const float Camera::DEFAULT_FOVX = 90.0f;
		const float Camera::DEFAULT_ZNEAR = 0.1f;
		const float Camera::DEFAULT_ZFAR = 1000.0f;

		const float Camera::DEFAULT_ORBIT_MIN_ZOOM = DEFAULT_ZNEAR + 1.0f;
		const float Camera::DEFAULT_ORBIT_MAX_ZOOM = DEFAULT_ZFAR * 0.5f;

		const float Camera::DEFAULT_ORBIT_OFFSET_DISTANCE = DEFAULT_ORBIT_MIN_ZOOM +
			(DEFAULT_ORBIT_MAX_ZOOM - DEFAULT_ORBIT_MIN_ZOOM) * 0.25f;

		const Vector3 Camera::WORLD_XAXIS(1.0f, 0.0f, 0.0f);
		const Vector3 Camera::WORLD_YAXIS(0.0f, 1.0f, 0.0f);
		const Vector3 Camera::WORLD_ZAXIS(0.0f, 0.0f, 1.0f);

		Camera::Camera()
		{
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

			eye.set(0.0f, 0.0f, 0.0f);
			savedEye.set(0.0f, 0.0f, 0.0f);
			target.set(0.0f, 0.0f, 0.0f);
			xAxis.set(1.0f, 0.0f, 0.0f);
			yAxis.set(0.0f, 1.0f, 0.0f);
			targetYAxis.set(0.0f, 1.0f, 0.0f);
			zAxis.set(0.0f, 0.0f, 1.0f);
			viewDir.set(0.0f, 0.0f, -1.0f);

			acceleration.set(0.0f, 0.0f, 0.0f);
			currentVelocity.set(0.0f, 0.0f, 0.0f);
			velocity.set(0.0f, 0.0f, 0.0f);

			orientation.identity();
			savedOrientation.identity();

			viewMatrix.identity();
			projMatrix.identity();
			viewProjMatrix.identity();
		}

		Camera::~Camera()
		{

		}

		void Camera::lookAt(const Vector3 &target)
		{
			lookAt(eye, target, yAxis);
		}

		void Camera::lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
		{
			this->eye = eye;
			this->target = target;

			zAxis = eye - target;
			zAxis.normalize();

			viewDir = -zAxis;

			xAxis = Vector3::cross(up, zAxis);
			xAxis.normalize();

			yAxis = Vector3::cross(zAxis, xAxis);
			yAxis.normalize();

			viewMatrix[0][0] = xAxis.x;
			viewMatrix[1][0] = xAxis.y;
			viewMatrix[2][0] = xAxis.z;
			viewMatrix[3][0] = -Vector3::dot(xAxis, eye);

			viewMatrix[0][1] = yAxis.x;
			viewMatrix[1][1] = yAxis.y;
			viewMatrix[2][1] = yAxis.z;
			viewMatrix[3][1] = -Vector3::dot(yAxis, eye);

			viewMatrix[0][2] = zAxis.x;
			viewMatrix[1][2] = zAxis.y;
			viewMatrix[2][2] = zAxis.z;
			viewMatrix[3][2] = -Vector3::dot(zAxis, eye);

			// Extract the pitch angle from the view matrix.
			accumPitchDegrees = Math::radiansToDegrees(asinf(viewMatrix[1][2]));

			orientation.fromMatrix(viewMatrix);
			updateViewMatrix();
		}

		void Camera::move(float dx, float dy, float dz)
		{
			// Moves the camera by dx world units to the left or right; dy
			// world units upwards or downwards; and dz world units forwards
			// or backwards.

			if (mode == ORBIT)
			{
				// Orbiting camera is always positioned relative to the
				// target position. See updateViewMatrix().
				return;
			}

			Vector3 eye = this->eye;
			Vector3 forwards;

			if (mode == FIRST_PERSON)
			{
				// Calculate the forwards direction. Can't just use the camera's local
				// z axis as doing so will cause the camera to move more slowly as the
				// camera's view approaches 90 degrees straight up and down.

				forwards = Vector3::cross(WORLD_YAXIS, xAxis);
				forwards.normalize();
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

		void Camera::move(const Vector3 &direction, const Vector3 &amount)
		{
			// Moves the camera by the specified amount of world units in the specified
			// direction in world space.

			if (mode == ORBIT)
			{
				// Orbiting camera is always positioned relative to the
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
			// Construct a projection matrix based on the horizontal field of view
			// 'fovx' rather than the more traditional vertical field of view 'fovy'.

			float e = 1.0f / tanf(Math::degreesToRadians(fovx) / 2.0f);
			float aspectInv = 1.0f / aspect;
			float fovy = 2.0f * atanf(aspectInv / e);
			float xScale = 1.0f / tanf(0.5f * fovy);
			float yScale = xScale / aspectInv;

			projMatrix[0][0] = xScale;
			projMatrix[0][1] = 0.0f;
			projMatrix[0][2] = 0.0f;
			projMatrix[0][3] = 0.0f;

			projMatrix[1][0] = 0.0f;
			projMatrix[1][1] = yScale;
			projMatrix[1][2] = 0.0f;
			projMatrix[1][3] = 0.0f;

			projMatrix[2][0] = 0.0f;
			projMatrix[2][1] = 0.0f;
			projMatrix[2][2] = (zfar + znear) / (znear - zfar);
			projMatrix[2][3] = -1.0f;

			projMatrix[3][0] = 0.0f;
			projMatrix[3][1] = 0.0f;
			projMatrix[3][2] = (2.0f * zfar * znear) / (znear - zfar);
			projMatrix[3][3] = 0.0f;

			viewProjMatrix = viewMatrix * projMatrix;

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
			// Rotates the camera based on its current mode.
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
			// using these rotation angles to rotate the camera. This method is usually
			// called when the camera is being rotated using an input device (such as a
			// mouse or a joystick). 

			headingDegrees *= rotationSpeed;
			pitchDegrees *= rotationSpeed;
			rollDegrees *= rotationSpeed;

			rotate(headingDegrees, pitchDegrees, rollDegrees);
		}

		void Camera::undoRoll()
		{
			// Undo any camera rolling by leveling the camera. When the camera is
			// orbiting this method will cause the camera to become level with the
			// orbit target.

			if (mode == ORBIT)
				lookAt(eye, target, targetYAxis);
			else
				lookAt(eye, eye + viewDir, WORLD_YAXIS);
		}

		void Camera::updatePosition(const Vector3 &direction, float elapsedTimeSec)
		{
			// Moves the camera using Newton's second law of motion. Unit mass is
			// assumed here to somewhat simplify the calculations. The direction vector
			// is in the range [-1,1].

			if (currentVelocity.magnitudeSq() != 0.0f)
			{
				// Only move the camera if the velocity vector is not of zero length.
				// Doing this guards against the camera slowly creeping around due to
				// floating point rounding errors.

				Vector3 displacement = (currentVelocity * elapsedTimeSec) +
					(0.5f * acceleration * elapsedTimeSec * elapsedTimeSec);

				// Floating point rounding errors will slowly accumulate and cause the
				// camera to move along each axis. To prevent any unintended movement
				// the displacement vector is clamped to zero for each direction that
				// the camera isn't moving in. Note that the updateVelocity() method
				// will slowly decelerate the camera's velocity back to a stationary
				// state when the camera is no longer moving along that direction. To
				// account for this the camera's current velocity is also checked.

				if (direction.x == 0.0f && Math::closeEnough(currentVelocity.x, 0.0f))
					displacement.x = 0.0f;

				if (direction.y == 0.0f && Math::closeEnough(currentVelocity.y, 0.0f))
					displacement.y = 0.0f;

				if (direction.z == 0.0f && Math::closeEnough(currentVelocity.z, 0.0f))
					displacement.z = 0.0f;

				move(displacement.x, displacement.y, displacement.z);
			}

			// Continuously update the camera's velocity vector even if the camera
			// hasn't moved during this call. When the camera is no longer being moved
			// the camera is decelerating back to its stationary state.

			updateVelocity(direction, elapsedTimeSec);
		}

		void Camera::zoom(float zoom, float minZoom, float maxZoom)
		{
			if (mode == ORBIT)
			{
				// Moves the camera closer to or further away from the orbit
				// target. The zoom amounts are in world units.

				orbitMaxZoom = maxZoom;
				orbitMinZoom = minZoom;

				Vector3 offset = eye - target;

				orbitOffsetDistance = offset.magnitude();
				offset.normalize();
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

		void Camera::setAcceleration(const Vector3 &acceleration)
		{
			this->acceleration = acceleration;
		}

		void Camera::setMode(Mode newMode)
		{
			// Switch to a new camera mode (i.e., mode).
			// This method is complicated by the fact that it tries to save the current
			// mode's state prior to making the switch to the new camera mode.
			// Doing this allows seamless switching between camera modes.

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

				Vector3 newEye = eye + zAxis * orbitOffsetDistance;
				Vector3 newTarget = eye;

				lookAt(newEye, newTarget, targetYAxis);
				break;
			}
		}

		void Camera::setCurrentVelocity(const Vector3 &currentVelocity)
		{
			this->currentVelocity = currentVelocity;
		}

		void Camera::setCurrentVelocity(float x, float y, float z)
		{
			this->currentVelocity.set(x, y, z);
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

		void Camera::setOrientation(const Quaternion &newOrientation)
		{
			Matrix4 m = newOrientation.toMatrix4();

			// Store the pitch for this new orientation.
			// First person and spectator modes limit pitching to
			// 90 degrees straight up and down.

			accumPitchDegrees = Math::radiansToDegrees(asinf(m[1][2]));

			// First person and spectator modes don't allow rolling.
			// Negate any rolling that might be encoded in the new orientation.

			orientation = newOrientation;

			if (mode == FIRST_PERSON || mode == SPECTATOR)
				lookAt(eye, eye + viewDir, WORLD_YAXIS);

			updateViewMatrix();
		}

		void Camera::setPosition(const Vector3 &newEye)
		{
			eye = newEye;
			updateViewMatrix();
		}

		void Camera::setPreferTargetYAxisOrbiting(bool preferTargetYAxisOrbiting)
		{
			// Determines the mode of Y axis rotations when the camera is
			// orbiting a target. When preferTargetYAxisOrbiting is true all
			// Y axis rotations are about the orbit target's local Y axis.
			// When preferTargetYAxisOrbiting is false then the camera's
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

		void Camera::setVelocity(const Vector3 &velocity)
		{
			this->velocity = velocity;
		}

		void Camera::setVelocity(float x, float y, float z)
		{
			velocity.set(x, y, z);
		}

		void Camera::rotateFirstPerson(float headingDegrees, float pitchDegrees)
		{
			// Implements the rotation logic for the first person style and
			// spectator style camera modes. Roll is ignored.

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

			Quaternion rot;

			// Rotate camera about the world y axis.
			// Note the order the quaternions are multiplied. That is important!
			if (headingDegrees != 0.0f)
			{
				rot.fromAxisAngle(WORLD_YAXIS, headingDegrees);
				orientation = rot * orientation;
			}

			// Rotate camera about its local x axis.
			// Note the order the quaternions are multiplied. That is important!
			if (pitchDegrees != 0.0f)
			{
				rot.fromAxisAngle(WORLD_XAXIS, pitchDegrees);
				orientation = orientation * rot;
			}
		}

		void Camera::rotateFlight(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// Implements the rotation logic for the flight style camera mode.

			accumPitchDegrees += pitchDegrees;

			if (accumPitchDegrees > 360.0f)
				accumPitchDegrees -= 360.0f;

			if (accumPitchDegrees < -360.0f)
				accumPitchDegrees += 360.0f;

			Quaternion rot;

			rot.fromHeadPitchRoll(headingDegrees, pitchDegrees, rollDegrees);
			orientation *= rot;
		}

		void Camera::rotateOrbit(float headingDegrees, float pitchDegrees, float rollDegrees)
		{
			// Implements the rotation logic for the orbit style camera mode.
			// Roll is ignored for target Y axis orbiting.
			//
			// Briefly here's how this orbit camera implementation works. Switching to
			// the orbit camera mode via the setBehavior() method will set the
			// camera's orientation to match the orbit target's orientation. Calls to
			// rotateOrbit() will rotate this orientation. To turn this into a third
			// person style view the updateViewMatrix() method will move the camera
			// position back 'orbitOffsetDistance' world units along the camera's
			// local z axis from the orbit target's world position.

			Quaternion rot;

			if (_preferTargetYAxisOrbiting)
			{
				if (headingDegrees != 0.0f)
				{
					rot.fromAxisAngle(targetYAxis, headingDegrees);
					orientation = rot * orientation;
				}

				if (pitchDegrees != 0.0f)
				{
					rot.fromAxisAngle(WORLD_XAXIS, pitchDegrees);
					orientation = orientation * rot;
				}
			}
			else
			{
				rot.fromHeadPitchRoll(headingDegrees, pitchDegrees, rollDegrees);
				orientation *= rot;
			}
		}

		void Camera::updateVelocity(const Vector3 &direction, float elapsedTimeSec)
		{
			// Updates the camera's velocity based on the supplied movement direction
			// and the elapsed time (since this method was last called). The movement
			// direction is in the range [-1,1].

			if (direction.x != 0.0f)
			{
				// Camera is moving along the x axis.
				// Linearly accelerate up to the camera's max speed.

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
				// Linearly accelerate up to the camera's max speed.

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
				// Linearly accelerate up to the camera's max speed.

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

			viewMatrix = orientation.toMatrix4();

			xAxis.set(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
			yAxis.set(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
			zAxis.set(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
			viewDir = -zAxis;

			if (mode == ORBIT)
			{
				// Calculate the new camera position based on the current
				// orientation. The camera must always maintain the same
				// distance from the target. Use the current offset vector
				// to determine the correct distance from the target.

				eye = target + zAxis * orbitOffsetDistance;
			}

			viewMatrix[3][0] = -Vector3::dot(xAxis, eye);
			viewMatrix[3][1] = -Vector3::dot(yAxis, eye);
			viewMatrix[3][2] = -Vector3::dot(zAxis, eye);
		}

		const glm::mat4 Camera::view() const {
			glm::mat4 view;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					view[i][j] = viewMatrix[i][j];
				}
			}
			return view;
		}

		const glm::mat4 Camera::projection() const {
			glm::mat4 projection;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					projection[i][j] = projMatrix[i][j];
				}
			}
			return projection;
		}
	}
}