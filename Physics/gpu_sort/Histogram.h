#pragma once

#include <string>
#include <array>
#include "../../GLUtil/include/ncl/gl/compute.h"
#include "../../GLUtil/include/ncl/gl/Shader.h"
#include "../../GLUtil/include/ncl/gl/shaders.h"
#include "const_data.h"

namespace ncl {
	namespace gl {
		namespace parallel {
			class Histogram : public Compute {
			public:
				Histogram(std::array<SortData, 4>& data, ConstBuffer& consts, GLsizeiptr size, unsigned workers)
					:Compute{ glm::ivec3{workers, 1, 1}, {}, histogram_count_comp_shader }
					, _data{ data }
					, _consts{ consts }
					, _size{ size}
					, _histogram{}
				{
					_histogram.allocate(workers * RADICES);
					_histogram.fill(0);
				}

				void preCompute() override {
					_histogram.sendToGPU();
					_consts.sendToGPU();
					_data[KEY_IN].bind(DATA + KEY_IN);
					_data[KEY_OUT].bind(DATA + KEY_OUT);
				}

				void postCompute() override {
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				}

				StorageBuffer<unsigned int>& histogram() {
					return _histogram;
				}

			private:
				std::array<SortData, 4>& _data;
				ConstBuffer& _consts;
				GLsizeiptr _size;
				StorageBuffer<GLuint> _histogram;
			};
		}
	}
}