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
			class Permute : public Compute {
			public:
				Permute(std::array<SortData, 4>& data, ConstBuffer& consts, StorageBuffer<GLuint>& histogram, GLsizeiptr size, unsigned workers)
					:Compute{ glm::ivec3{workers, 1, 1}, {}, permute_comp_shader }
					, _data{ data }
					, _consts{ consts }
					, _size{ size }
					, _histogram{ histogram }
				{

				}

				void preCompute() override {
					_histogram.sendToGPU();
					_consts.sendToGPU();
					_data[KEY_IN].bind(DATA + KEY_IN);
					_data[KEY_OUT].bind(DATA + KEY_OUT);
					_data[VALUE_IN].bind(DATA + VALUE_IN);
					_data[VALUE_OUT].bind(DATA + VALUE_OUT);
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
				StorageBuffer<GLuint>& _histogram;
			};
		}
	}
}