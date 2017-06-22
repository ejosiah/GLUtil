#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ncl {
	namespace gl {
		struct Material {
			glm::vec4 ambient{ 0 };
			glm::vec4 diffuse{ 0.8, 0.8, 0.8, 1 };
			glm::vec4 specular{ 1 };
			glm::vec4 transmittance{ 0 };
			glm::vec4 emission{ 0 };
			float shininess{ 50 };
			float ior{ 1 };      // index of refraction
			float opacity{ 1 }; // 1 == opaque; 0 == fully transparent
			int diffuseMat = -1;
			int specularMat = -1;
			int ambientMat = -1;
			int bumpMap = -1;
		};

		struct LightSource {
			glm::vec4 position{ 0, 0, 1, 0 };
			glm::vec4 ambient{ 0 };
			glm::vec4 diffuse{ 1 };
			glm::vec4 specular{ 1 };
			glm::vec4 spotDirection{ 0, 0, -1, 0 };
			float spotAngle = 180.0f;
			float spotExponent = 2.0f;
			float kc = 1;
			float ki = 0;
			float kq = 0;
			bool transform = false;
			bool on = false;
		};

		struct LightModel {
			bool localViewer = false;
			bool twoSided = false;
			bool useObjectSpace = false;
			bool celShading = false;
			bool colorMaterial = false;
			glm::vec4 globalAmbience{ 0.02, 0.02, 0.02, 1 };
		};

		Material DEFAULT_MATERIAL;
	}
}
