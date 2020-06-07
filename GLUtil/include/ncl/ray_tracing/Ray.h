#pragma once

#include <glm/glm.hpp>
#include "..\gl\StorageBufferObj.h"
#include <vector>

namespace ncl {
	namespace ray_tracing {
#pragma pack(push, 1)
		struct Ray {
			glm::vec4 origin = glm::vec4(0);
			glm::vec4 direction = glm::vec4(0);
			float tMax = 0.0f;
			glm::vec3 padding;
		};
#pragma pack(pop)
	}
}

//template<>
//struct ncl::gl::ObjectReflect<std::vector<ncl::ray_tracing::Ray>> {
//
//	static GLsizeiptr sizeOfObj(std::vector<ncl::ray_tracing::Ray>& rays) {
//		auto size = sizeof(ncl::ray_tracing::Ray);
//		return GLsizeiptr(size * rays.size());
//	}
//
//	static void* objPtr(std::vector<ncl::ray_tracing::Ray>& rays) {
//		return &rays[0];
//	}
//
//	static GLsizeiptr sizeOf(int count) {
//		return GLsizeiptr(sizeof(ncl::ray_tracing::Ray) * count);
//	}
//};