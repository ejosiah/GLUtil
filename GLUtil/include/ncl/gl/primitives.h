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
	}
}