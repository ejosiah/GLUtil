#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../gl/common.h"
#include "Ray.h"
#include "camera.h"

namespace ncl {
	namespace ray_tracing {

		static const int BSDF_REFLECTION = 1 << 0;
		static const int BSDF_TRANSMISSION = 1 << 1;
		static const int BSDF_DIFFUSE = 1 << 2;
		static const int BSDF_GLOSSY = 1 << 3;
		static const int BSDF_SPECULAR = 1 << 4;
		static const int BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION;

		const int SPECIULAR_REFLECT = BSDF_REFLECTION | BSDF_SPECULAR | (1 << 5);
		const int SPECULAR_TRANSMISSION = BSDF_TRANSMISSION | BSDF_SPECULAR | (1 << 6);
		const int LAMBERTIAN_REFLECT = BSDF_REFLECTION | BSDF_DIFFUSE | (1 << 7);
		const int OREN_NAYAR = BSDF_REFLECTION | BSDF_DIFFUSE | (1 << 8);
		const int FRESNEL_SPECULAR = BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR | (1 << 9);

		static const int FRESNEL_NOOP = 1 << 0;
		static const int FRESNEL_DIELECTRIC = 1 << 1;
		static const int FRESNEL_CONDOCTOR = 1 << 2;

		const int DELTA_POSITION = 1 << 0;
		const int DELTA_DIRECTION = 1 << 2;
		const int AREA_LIGHT = 1 << 3;
		const int INFINITE_LIGHT = 1 << 4;
		const int POINT_LIGHT = (1 << 5) | DELTA_POSITION;
		const int SUN_LIGHT = (1 << 6) | DELTA_DIRECTION;
		const int DISTANT_LIGHT = (1 << 7) | DELTA_DIRECTION;

#pragma pack(push, 1)
		struct Plane {
			glm::vec4 n;
			glm::vec4 min = glm::vec4(-1000);
			glm::vec4 max = glm::vec4(1000);
			float d;
			int id;
			int matId;
			int padding;
		//	glm::glm::vec2 padding;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Sphere {
			glm::vec4 center;
			glm::vec4 color;
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
			int padding;
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

#pragma pack(push, 1)
		struct Triangle {
			glm::vec3 a;
			padding_4 padding0;

			glm::vec3 b;
			padding_4 padding1;

			glm::vec3 c;
			padding_4 padding2;

			int objectToWorldId;
			int worldToObjectId;
			int id;
			int matId;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Shading {
			glm::vec3 n0;
			int id;
			
			glm::vec3 n1;
			padding_4 padding0;

			glm::vec3 n2;
			padding_4 padding1;

			glm::vec3 t0;
			padding_4 padding2;

			glm::vec3 t1;
			padding_4 padding3;

			glm::vec3 t2;
			padding_4 padding4;

			glm::vec3 bi0;
			padding_4 padding5;

			glm::vec3 bi1;
			padding_4 padding6;

			glm::vec3 bi2;
			padding_4 padding7;

			glm::vec2 uv0;
			glm::vec2 uv1;

			glm::vec2 uv2;
			padding_8 padding8;
		};
#pragma pack(pop)
	}
}