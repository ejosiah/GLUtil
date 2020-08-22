#pragma once

#include <gl/gl_core_4_5.h>

namespace ncl {

	template<typename T>
	struct Wrapper {
		T t;
	};

	using BufferId = Wrapper<GLuint>;
	using TextureId = Wrapper<GLuint>;
	using UnitId = Wrapper<GLuint>;
}