#pragma once

#include "Cube.h"
#include "mesh.h"
#include "Model.h"
#include "models.h"
#include "Sphere.h"
#include "Hemisphere.h"
#include "Plane.h"
#include "ProvidedMesh.h"
#include "Teapot.h"
#include "PatchTeapot.h"
#include "Cylinder.h"
#include "Cone.h"
#include "CrossHair.h"
#include "Vector.h"
#include "DoubleBufferedObj.h"
#include "AABBShape.h"
#include "../geom/aabb2.h"


namespace ncl {
	namespace gl {

		inline Mesh screnSpaceQuad() {
			gl::Mesh mesh;

			mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
			mesh.positions.emplace_back(-1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, 1.0f, 0.0f);

			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(0.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 1.0f);

			return mesh;
		}

		inline Mesh worldSpaceQuad() {
			gl::Mesh mesh;

			mesh.positions.emplace_back(-1.0f, 0.0f, 1.0f);
			mesh.positions.emplace_back(-1.0f, 0.0f, -1.0f);
			mesh.positions.emplace_back(1.0f, 0.0f, -1.0f);
			mesh.positions.emplace_back(-1.0f, 0.0f, 1.0f);
			mesh.positions.emplace_back(1.0f, 0.0f, -1.0f);
			mesh.positions.emplace_back(1.0f, 0.0f, 1.0f);

			mesh.normals.emplace_back(0, 1, 0);
			mesh.normals.emplace_back(0, 1, 0);
			mesh.normals.emplace_back(0, 1, 0);
			mesh.normals.emplace_back(0, 1, 0);
			mesh.normals.emplace_back(0, 1, 0);
			mesh.normals.emplace_back(0, 1, 0);

			mesh.tangents.emplace_back(1, 0, 0);
			mesh.tangents.emplace_back(1, 0, 0);
			mesh.tangents.emplace_back(1, 0, 0);
			mesh.tangents.emplace_back(1, 0, 0);
			mesh.tangents.emplace_back(1, 0, 0);
			mesh.tangents.emplace_back(1, 0, 0);

			// TODO check this might be wrong may need to point in [0, 0, -1]
			mesh.bitangents.emplace_back(0, 0, 1);
			mesh.bitangents.emplace_back(0, 0, 1);
			mesh.bitangents.emplace_back(0, 0, 1);
			mesh.bitangents.emplace_back(0, 0, 1);
			mesh.bitangents.emplace_back(0, 0, 1);
			mesh.bitangents.emplace_back(0, 0, 1);

			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(0.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 1.0f);

			return mesh;
		}

		inline ProvidedMesh aabbOutline(const geom::bvol::AABB2& aabb, const glm::vec4 color = randomColor()) {
			using namespace glm;
			using namespace std;
			Mesh mesh;
			mesh.positions.push_back(vec3(aabb.min.x, aabb.min.y, aabb.max.z));
			mesh.positions.push_back(vec3(aabb.min.x, aabb.min.y, aabb.min.z));
			mesh.positions.push_back(vec3(aabb.min.x, aabb.max.y, aabb.min.z));
			mesh.positions.push_back(vec3(aabb.min.x, aabb.max.y, aabb.max.z));
			mesh.positions.push_back(vec3(aabb.max.x, aabb.min.y, aabb.min.z));
			mesh.positions.push_back(vec3(aabb.max.x, aabb.max.y, aabb.min.z));
			mesh.positions.push_back(vec3(aabb.max.x, aabb.min.y, aabb.max.z));
			mesh.positions.push_back(vec3(aabb.max.x, aabb.max.y, aabb.max.z));


			GLuint indices[] = {
				3, 2, 2, 5, 5,
				7, 7, 3, 3, 0,
				0, 6, 6, 4, 4,
				1, 1, 0, 0, 3,
				5, 4, 2, 1, 7,
				6
			};

			mesh.indices = vector<GLuint>(begin(indices), end(indices));
			mesh.colors = vector<vec4>(8, color);
			mesh.primitiveType = GL_LINES;

			return ProvidedMesh{ mesh };
		}

		inline ProvidedMesh aabbOutline(const Shape& shape, const glm::vec4 color = randomColor()) {
			return aabbOutline(shape.aabb(), color);
		}

	}
}