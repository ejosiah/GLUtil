#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>

namespace ncl { namespace gl{
    
	const size_t MAX_UVS = 8;

	struct Mesh {
		std::string name;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<glm::vec4> colors;
		std::array<std::vector<glm::vec2>, MAX_UVS> uvs;
		std::vector<unsigned int> indices;
		Material material;
        GLenum primitiveType = GL_TRIANGLES;

		bool hasNormals() { return !normals.empty(); }
		bool hasTangents() { return !tangents.empty(); }
		bool hasColors() { return !colors.empty(); }
		bool hasTexCoords() { return std::any_of(uvs.begin(), uvs.end(), [](std::vector<glm::vec2>& uvs) { return !uvs.empty(); }); }
		bool hasIndices() { return !indices.empty(); }

		int numTexCoords() {
			return std::count_if(uvs.begin(), uvs.end(), [](std::vector<glm::vec2>& uvs) { return !uvs.empty(); });
		}

		void clear() {
			positions.clear();
			normals.clear();
			tangents.clear();
			bitangents.clear();
			colors.clear();
			for (auto& uv : uvs) { uv.clear(); }
			indices.clear();
		}
	};

	struct Mesurements {
		glm::vec3 min, max;
		float height;
		float width;
		float length;

		Mesurements() {}
		
		Mesurements(glm::vec3 min, glm::vec3 max) {
			this->min = min;
			this->max = max;
			height = glm::length(max.y - min.y);
			width = glm::length(max.x - min.x);
			length = glm::length(max.z - min.z);
		}

		Mesurements(float w, float h, float l = 0.0f) {
			width = w;
			length = l;
			height = h;
		}
	};

}}
