#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>

namespace ncl { namespace gl{
    
	const size_t MAX_UVS = 8;

	struct Mesh {
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
	};

	struct Mesurements {
		glm::vec3 minX, minY, minZ, maxX, maxY, maxZ;	// TODO use 2 vectors instead
		float height;
		float width;
		float length;

		Mesurements() {}
		
		Mesurements(glm::vec3 minX, glm::vec3 minY, glm::vec3 minZ, glm::vec3 maxX, glm::vec3 maxY, glm::vec3 maxZ) {
			this->minX = minX;
			this->minY = minY;
			this->minZ = minZ;
			this->maxX = maxX;
			this->maxY = maxY;
			this->maxZ = maxZ;
			height = glm::length(maxY - minY);
			width = glm::length(maxX - minY);
			length = glm::length(maxZ - minZ);
		}

		Mesurements(float w, float h, float l = 0.0f) {
			width = w;
			length = l;
			height = h;
		}
	};

}}
