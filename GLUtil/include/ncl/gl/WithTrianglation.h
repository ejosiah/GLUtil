#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "logger.h"

namespace ncl {
	namespace gl {
		class WithTriangulation {
		public:
			std::vector<unsigned> triangulate(std::vector<unsigned>& indices, unsigned n) {
				using namespace std;
				size_t size = indices.size();
				std::vector<unsigned> triangulatedIndices;
				for (int i = 3; i < size - 2; i++) {
					if (i % 2 == 0) {
						triangulatedIndices.push_back(i - 3);
						triangulatedIndices.push_back(i - 1);
						triangulatedIndices.push_back(i - 2);

					}
					else {
						triangulatedIndices.push_back(i - 3);
						triangulatedIndices.push_back(i - 2);
						triangulatedIndices.push_back(i - 1);
					}
				}
				return triangulatedIndices;
			}



		private:
			ncl::Logger logger = ncl::Logger::get("WithTriangulation");
		};
	}
}