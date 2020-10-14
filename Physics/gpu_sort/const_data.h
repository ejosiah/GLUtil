#pragma once

#include <any>
#include "../../GLUtil/include/ncl/gl/BufferObject.h"

namespace ncl {
	namespace gl {
		namespace parallel {

			constexpr GLuint WG_COUNT = 1;
			constexpr GLuint RADICES = 16;
			constexpr GLuint HISTOGRAM = 0;
			constexpr GLuint CONSTS = 0;
			constexpr GLuint DATA = 1;
			constexpr GLuint KEY_IN = 0;
			constexpr GLuint KEY_OUT = 1;
			constexpr GLuint VALUE_IN = 2;
			constexpr GLuint VALUE_OUT = 3;

#pragma pack(push, 1)
			struct Consts {
				unsigned int shift;
				unsigned int descending;
				unsigned int is_signed;
				unsigned int key_index;
			};
#pragma pack(pop)

			using SortData = StorageBuffer<GLint>;
			using ConstBuffer = UniformBuffer<Consts>;
		}
	}
}