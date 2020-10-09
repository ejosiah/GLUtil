#pragma once

#include <string>
#include <array>
#include "../../GLUtil/include/ncl/gl/compute.h"
#include "../../GLUtil/include/ncl/gl/Shader.h"
#include "const_data.h"

namespace ncl {
	namespace gl {
		namespace parallel {
			class Histogram : public Compute {
			public:
				Histogram(std::array<SortData, 4>& data, ConstBuffer& consts, GLsizeiptr size, Shader* shader, unsigned workers)
					:Compute{ glm::ivec3{workers, 1, 1}, {}, shader }
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
					_data[KEY_IN].bind(DATA + KEY_IN, 0, _size);
					_data[KEY_OUT].bind(DATA + KEY_OUT, 0, _size);
					_data[VALUE_IN].bind(DATA + VALUE_IN, 0, _size);
					_data[VALUE_OUT].bind(DATA + VALUE_OUT, 0, _size);
				}

				void postCompute() override {
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				}

				const StorageBuffer<unsigned int>& histogram() const {
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