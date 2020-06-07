#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "..\gl\Scene.h"
#include "..\gl\StorageBufferObj.h"

namespace ncl {
	namespace ray_tracing {
		struct Camera {
			glm::mat4 cameraToWorld = glm::mat4(1);
			glm::mat4 cameraToScreen = glm::mat4(1);
			glm::mat4 rasterToCamera = glm::mat4(1);
			glm::mat4 screenToRaster = glm::mat4(1);
			glm::mat4 rasterToScreen = glm::mat4(1);
			float shutterOpen = 0.f;
			float shutterClose = 1.f;
			float lensRadius = 0.f;
			float focalDistance = 1000000;
		};

		glm::mat4 rasterToScreen(float w, float h) {
			glm::mat4 mat = glm::translate(glm::mat4(1), glm::vec3(-1, -1, 0));
			mat = scale(mat, { 2 / w, 2 / h, 1 });
			return mat;
		}

		void update(Camera& camera, gl::Scene& scene) {
			camera.rasterToCamera = glm::mat4{ 1 };
			camera.rasterToScreen = rasterToScreen(scene.width(), scene.height());
			camera.screenToRaster = glm::inverse(camera.rasterToCamera);
			camera.cameraToScreen = scene.activeCamera().getProjectionMatrix();
			camera.rasterToCamera = glm::inverse(scene.activeCamera().getProjectionMatrix()) * camera.rasterToScreen;
			camera.cameraToWorld = glm::inverse(scene.activeCamera().getViewMatrix());
		}
	}
}

//template<>
//struct ncl::gl::ObjectReflect<ncl::ray_tracing::Camera> {
//
//	static GLsizeiptr sizeOfObj(ncl::ray_tracing::Camera& camera) {
//		return GLsizeiptr(sizeof(camera));
//	}
//
//	static void* objPtr(ncl::ray_tracing::Camera& camera) {
//		return &camera;
//	}
//};
