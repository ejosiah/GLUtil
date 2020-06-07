#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "Ray.h"
#include "camera.h"

namespace ncl {
	namespace ray_tracing {

		const int DELTA_POSITION = 1 << 0;
		const int DELTA_DIRECTION = 1 << 2;
		const int AREA_LIGHT = 1 << 3;
		const int INFINITE_LIGHT = 1 << 4;
		const int POINT_LIGHT = (1 << 5) | DELTA_POSITION;

#pragma pack(push, 1)
		struct Plane {
			glm::vec3 n;
			float d;
			int id;
			int matId;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Sphere {
			glm::vec3 center;
			glm::vec3 color;
			glm::mat4 objectToWorld = glm::mat4(1);
			glm::mat4 worldToObject = glm::mat4(1);
			float radius = 1;
			float yMin = -100;
			float yMax = 100;
			float thetaMin = 0;
			float thetaMax = glm::pi<float>();
			float phiMax = glm::two_pi<float>();
			int id;
			int matId;
		};
#pragma pack(pop)	

#pragma pack(push, 1)
		struct Material {
			glm::vec4 ambient;
			glm::vec4 diffuse;
			glm::vec4 specular;
			glm::vec4 kr = glm::vec4(-1);
			glm::vec4 kt = glm::vec4(-1);
			float shine;
			float ior;
			int nBxDfs = 0;
			int bsdf[8];
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct LightSource {
			glm::vec4 I;
			glm::vec4 position;
			glm::mat4 lightToWorld;
			glm::mat4 worldToLight;
			int flags;
			int nSamples = 1;
			int shapeId = 1;
		};
#pragma pack(pop)

	}
}