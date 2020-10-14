#pragma once

#include <string>
#include <array>
#include <memory>
#include "../../GLUtil/include/ncl/gl/compute.h"
#include "../../GLUtil/include/ncl/gl/Shader.h"
#include "const_data.h"
#include "Histogram.h"
#include "PrefixSum.h"
#include "Permute.h"

namespace ncl {
	namespace gl {
		namespace parallel {
			class RadixSort : public Compute {
			public:
				RadixSort(StorageBuffer<GLuint>& input, ConstBuffer& consts, unsigned workers)
					:Compute{ glm::ivec3{workers, 1, 1}, {}, permute_comp_shader }
					, _consts{ consts }
				{
					_data[KEY_IN].reference(input.buffer());
					_data[KEY_OUT].allocate(input.size(), DATA + KEY_OUT);
					_data[VALUE_IN].empty();
					_data[VALUE_OUT].empty();
					_histogram = std::make_unique<Histogram>(_data, consts, _data.size(), WG_COUNT);
					_prefixSum = std::make_unique<PrefixSum>(_histogram->histogram(), WG_COUNT);
					_permute = std::make_unique<Permute>(_data, consts, _histogram->histogram(), _data.size(), WG_COUNT);
				}

				void preCompute() override {
					_histogram->preCompute();
					_prefixSum->preCompute();
					_permute->preCompute();
				}



				void compute() override {
					_histogram->compute();
					_prefixSum->compute();
					_permute->compute();
				//	Compute::compute();
				}

				void postCompute() override {
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
					std::stringstream sbr;
					sbr.str("");
					sbr.clear();
					_data[KEY_OUT].read([&sbr](auto ptr) {
						for (int i = 0; i < 64; i++) sbr << *(ptr + i) << " ";
						});
					logger.info("data after:\t" + sbr.str());
					std::swap(_data[KEY_IN], _data[KEY_OUT]);
					std::swap(_data[VALUE_IN], _data[VALUE_OUT]);
				}

			private:
				std::array<SortData, 4> _data;
				ConstBuffer& _consts;
				std::unique_ptr<Histogram> _histogram;
				std::unique_ptr<PrefixSum> _prefixSum;
				std::unique_ptr<Permute> _permute;
				ncl::Logger logger = Logger::get("sort");
			};
		}
	}
}