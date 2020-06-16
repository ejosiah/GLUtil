#pragma once

#include "..\gl\compute.h"
#include "..\gl\Scene.h"
#include "Ray.h"
#include "camera.h"
#include <vector>
#include <glm/glm.hpp>

#pragma pack(push, 1)
struct Ray0 {
	Ray0() = default;
	Ray0(glm::vec3 o, glm::vec3 d, float t) 
		:origin{ o }, direction{ d }, tMax{ 4 }{

	}
	glm::vec3 origin;
	ncl::padding_4 padding;
	glm::vec3 direction;
	float tMax;
	glm::vec2 uv;
	ncl::padding_8 padding1;
};
#pragma pack(pop)

namespace ncl {
	namespace ray_tracing {
		class RayGenerator : public gl::Compute {
		public:
			RayGenerator(){}

			RayGenerator(gl::Scene& scene, gl::StorageBufferObj<Camera>& camera_ssbo)
				: Compute{ glm::vec3{ scene.width() / 32.0f, scene.height() / 32.0f, 1.0f }
					, {}
					, &scene.shader("generate_rays") }
				, scene{ &scene }
				, camera_ssbo{ &camera_ssbo }
				, numRays{ scene.width() * scene.height() }
			{
				rays = gl::StorageBufferObj<Ray>{ numRays, 1 };
				rays0 = gl::StorageBufferObj<Ray0>{ numRays, 12 };
				scene.shader("generate_rays").use([&] {
					rays.sendToGPU(false);
					rays0.sendToGPU(false);
				});
			}

			void preCompute() override {
				update(*camera_ssbo->get(), *scene);
				auto cam = camera_ssbo->get();
				camera_ssbo->sendToGPU();
				//rays.sendToGPU(false);
			}

			bool once = true;
			void postCompute() override {
				//if (once) {
				//	once = false;
				//	rays.read([&](Ray* ptr) {
				//		std::stringstream ss;
				//		for (int i = 0; i < 10; i++) {
				//			auto ray = *(ptr + i);
				//			ss << "Ray[ ";
				//			ss << "o: " << ray.origin;
				//			ss << ", d: " << ray.direction;
				//			ss << ", t: " << ray.tMax;
				//			ss << " ]";
				//			logger.info(ss.str());
				//			ss.clear();
				//			ss.str("");
				//		}
				//		});
				//	logger.info("");
				//	rays0.read([&](Ray0* ptr) {
				//		std::stringstream ss;
				//		for (int i = 0; i < 10; i++) {
				//			auto ray = *(ptr + i);
				//			ss << "Ray[ ";
				//			ss << "o: " << glm::vec4(ray.origin, 1);
				//			ss << ", d: " << glm::vec4(ray.direction, 1);
				//			ss << ", t: " << ray.tMax;
				//			ss << ", uv: " << ray.uv;
				//			ss << " ]";
				//			logger.info(ss.str());
				//			ss.clear();
				//			ss.str("");
				//		}
				//		});
				//}
			}

			Ray* getRays() {
				return rays.get();
			}

			gl::StorageBufferObj<Ray>& getRaySSBO() {
				return rays;
			}

		private:
			gl::Scene* scene;
			gl::StorageBufferObj<Camera>* camera_ssbo;
			gl::StorageBufferObj<Ray> rays;
			gl::StorageBufferObj<Ray0> rays0;
			int numRays;
			Logger logger = Logger::get("ray");
		};
	}
}