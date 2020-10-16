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
			class RadixSort {
			public:
				RadixSort(StorageBuffer<GLuint>& input, bool descending = false, bool isSigned = false, bool isFloat = false)
				{

					_consts.allocate();
					_nextConst[0] = { 0, descending, 0, false };
					_nextConst[1] = { 4, descending, 0, false };
					_nextConst[2] = { 8, descending, 0, false };
					_nextConst[3] = { 12, descending, 0, false };
					_nextConst[4] = { 16, descending, 0, false };
					_nextConst[5] = { 20, descending, 0, false };
					_nextConst[6] = { 24, descending, 0, false };
					_nextConst[7] = { 28, descending, isSigned && !isFloat, false };
					_nextConst[8] = { 28, descending, 1, false };


					_data[KEY_IN].reference(input.buffer());
					_data[KEY_OUT].allocate(input.count(), DATA + KEY_OUT);
					_data[VALUE_IN].empty();
					_data[VALUE_OUT].empty();
					_histogram = std::make_unique<Histogram>(_data, _consts, WG_COUNT * RADICES, WG_COUNT);
					_prefixSum = std::make_unique<PrefixSum>(_histogram->histogram());
					_permute = std::make_unique<Permute>(_data, _consts, _histogram->histogram(), _data.size(), WG_COUNT);
				}



				void execute() {

					for (int i = 0; i < 8; i++) {
							_consts.update(&_nextConst[i]);

						_histogram->compute();
						_prefixSum->compute();
						_permute->compute();

						//if (i == 0) {
						//	postCompute();
						//}

						std::swap(_data[KEY_IN], _data[KEY_OUT]);
						std::swap(_data[VALUE_IN], _data[VALUE_OUT]);
					}
				}

				void postCompute() {
					std::stringstream sbr;
					sbr.str("");
					sbr.clear();
					_data[KEY_IN].read([&sbr](auto ptr) {
						for (int i = 0; i < 64; i++) sbr << *(ptr + i) << " ";
						});
					//_histogram->histogram().read([&sbr](auto ptr) {
					//	for (int i = 0; i < (RADICES * WG_COUNT); i++) 
					//		sbr << *(ptr + i) << ((i+1)%RADICES == 0 ? "\n" : ", ");
					//	});
					logger.info("data after:\t\n" + sbr.str());

				}

			private:
				std::array<SortData, 4> _data;
				ConstBuffer _consts;
				std::array<Consts, 9> _nextConst;
				std::unique_ptr<Histogram> _histogram;
				std::unique_ptr<PrefixSum> _prefixSum;
				std::unique_ptr<Permute> _permute;
				ncl::Logger logger = Logger::get("sort");
			};
		}
	}
}