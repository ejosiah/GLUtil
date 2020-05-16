#pragma once

#include <glm/glm.hpp>
#include "textures.h";

namespace ncl {
	namespace gl {

		template<size_t N>
		struct ProbeGrid {
			Texture texture;
			glm::vec4 size;
			glm::vec4 invSize;
			glm::vec4 readMultiplyFirst;
			glm::vec4 readAddSecond;
			bool notNull;
			const size_t dim = N;
		};

		using ProbeGrid2DArray = ProbeGrid<2>;
		using probeGridCubeArray = ProbeGrid<2>;

		struct LightFieldSurface {
			ProbeGrid2DArray radianceProbeGrid;
			ProbeGrid2DArray normalProbeGrid;
			ProbeGrid2DArray distanceProbeGrid;
			ProbeGrid2DArray lowResolutionDistanceProbeGrid;
			glm::ivec3 probeCounts;
			glm::vec3 probeStartPosition;
			glm::vec3 probeStep;
			int lowResolutionDownsampleFactor;
			probeGridCubeArray irradianceProbeGrid;
			probeGridCubeArray meanDistProbeGrid;
		};
	}
}