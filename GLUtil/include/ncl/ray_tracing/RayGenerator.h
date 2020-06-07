#pragma once

#include "..\gl\compute.h"
#include "..\gl\Scene.h"
#include "Ray.h"
#include "camera.h"
#include <vector>

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
				rays = gl::StorageBufferObj<std::vector<Ray>>{ numRays, 1 };
				scene.shader("generate_rays").use([&] {
					rays.sendToGPU(false);
				});
			}

			void preCompute() override {
				update(camera_ssbo->get(), *scene);
				camera_ssbo->sendToGPU();
				rays.sendToGPU(false);
			}

			std::vector<Ray>& getRays() {
				return rays.get();
			}

			gl::StorageBufferObj<std::vector<Ray>>& getRaySSBO() {
				return rays;
			}

		private:
			gl::Scene* scene;
			gl::StorageBufferObj<Camera>* camera_ssbo;
			gl::StorageBufferObj<std::vector<Ray>> rays;
			int numRays;
		};
	}
}