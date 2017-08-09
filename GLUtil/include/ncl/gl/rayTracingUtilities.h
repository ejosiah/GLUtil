#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "common.h"

namespace ncl {
	namespace gl {
		namespace ray_tracing {


#pragma pack (push, 1)
			struct Material {
				glm::vec4 ambient;
				glm::vec4 diffuse;
				glm::vec4 specular;
				glm::vec4 transmittance;
				glm::vec4 emission;
				float shininess;
				float ior;
				float opacity;
			};
#pragma pack (pop)

#pragma pack (push, 1)
			struct Plane {
				glm::vec4 n;
				glm::vec4 ambient;
				glm::vec4 diffuse;
				glm::vec4 specular;
				glm::vec4 transmittance;
				glm::vec4 emission;
				float shininess;
				float ior;
				float opacity;
				float f0 = 1.0f;
				float d;
			};
#pragma pack (pop)

#pragma pack (push, 1)
			struct Triangle {
				glm::vec4 v0;
				glm::vec4 v1;
				glm::vec4 v2;
				Material material;
				float tid;
			};
#pragma pack (pop)

#pragma pack (push, 1)
			struct Sphere {
				glm::vec4 center;
				glm::vec4 ambient;
				glm::vec4 diffuse;
				glm::vec4 specular;
				glm::vec4 transmittance;
				glm::vec4 emission;
				float shininess;
				float ior;
				float opacity;
				float f0;
				float radius;
			};
#pragma pack (pop)

#pragma pack (push, 1)
			struct Box {
				glm::vec4 min;
				glm::vec4 max;
				Material material;
			};
#pragma pack (pop)

			struct SSBOTriangleData {
				std::vector<Triangle> triangles;
			};

			unsigned int sizeOf(SSBOTriangleData& ssbo) {
				return sizeof(Triangle)  * ssbo.triangles.size();
			}

			// TODO extract data from Model
		}
	}
}