#pragma once

#include <array>
#include "../../GLUtil/include/ncl/gl/compute.h"
#include "../../GLUtil/include/ncl/gl/Shader.h"
#include "../../GLUtil/include/ncl/gl/shaders.h"
#include "const_data.h"

namespace ncl {
	namespace gl {
		namespace parallel {
			class PrefixSum : public Compute {
			public:
				PrefixSum(StorageBuffer<GLuint>& histogram)
					: Compute{ glm::ivec3{1}, {}, prefix_scan_comp_shader }
					, _histogram{ histogram }
				{
				}

				void preCompute() override {
					_histogram.sendToGPU();
				}

				void postCompute() override {
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				}

			private:
				StorageBuffer<GLuint>& _histogram;
			};
		}
	}
}