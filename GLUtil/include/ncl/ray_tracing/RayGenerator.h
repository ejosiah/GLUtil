#pragma once

#include "..\gl\compute.h"
#include "..\gl\Scene.h"
#include "Ray.h"
#include "camera.h"
#include <vector>
#include <glm/glm.hpp>

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
				, numRays{ size_t(scene.width() * scene.height()) }
			{
				rays = gl::StorageBufferObj<Ray>{ numRays, 1 };
				scene.shader("generate_rays").use([&] {
					rays.sendToGPU(false);
				});
			}

			void preCompute() override {
				update(*camera_ssbo->get(), *scene);
				auto cam = camera_ssbo->get();
				camera_ssbo->sendToGPU();
				//rays.sendToGPU(false);
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
			size_t numRays;
		};
	}
}