#pragma once

namespace ncl {
	namespace geom {
		namespace bvol {
			class BoundingVolume {
			public:
				virtual bool test(const BoundingVolume* other) const = 0;
			};
		}
	}
}