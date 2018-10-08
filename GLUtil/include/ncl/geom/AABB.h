#pragma once

#include <limits>
#include "bounding_volumn.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace ncl {
	namespace geom {
		namespace bvol {
			class AABB : public BoundingVolume {
			public:
				AABB(glm::vec3* points, int n){
					_min = _max = *points;
					for (int i = 1; i < n; i++) {
						_min = glm::min(_min, *(points + i));
						_max = glm::max(_max, *(points + i));
					}
				}

				AABB(glm::vec3 min, glm::vec3 max) :_min(min), _max(max) {

				}

				virtual bool test(const BoundingVolume* other) const override {
					if (dynamic_cast<const AABB*>(other) != nullptr) {
						return test(dynamic_cast<const AABB*>(other));
					}

					return false;
				}

				bool test(const AABB& b) {
					if (_max.x < b._min.x || _min.x > b._max.x) return false;
					if (_max.y < b._min.y || _min.y > b._max.y) return false;
					if (_max.z < b._min.z || _min.z > b._max.z) return false;

					return true;
				}

				glm::vec3 closestPoint(glm::vec3 p) {
					glm::vec3 q;
					for (int i = 0; i < 3; i++) {
						q[i] = p[i];
						if (q[i] < _min[i]) q[i] = _min[i];
						if (q[i] > _max[i]) q[i] = _max[i];
					}
					return q;
				}

				float sqDistance(glm::vec3 p) {
					float d = 0;
					for (int i = 0; i < 3; i++) {
						float v = p[i];
						if (v < _min[i]) d += (_min[i] - v) * (_min[i] - v);
						if (v > _max[i]) d += (v - _max[i]) * (v - _max[i]);
					}
					return d;
				}

				const glm::vec3& min() const {
					return _min;
				}
				
				const glm::vec3& max() const {
					return _max;
				}

			private:
				glm::vec3 _min;
				glm::vec3 _max;
			};
		}
	}
}