#pragma once
#include "../gl/primitives.h"
#include "AABB.h"

namespace ncl {
	namespace geom {
		namespace bvol {

			gl::Drawable* visualize(const BoundingVolume* volume) {
				if (dynamic_cast<const AABB*>(volume) != nullptr) {
					const AABB& aabb = *dynamic_cast<const AABB*>(volume);
					using namespace gl;
					using namespace glm;
					using namespace std;
					Mesh mesh;
					mesh.positions.push_back(vec3(aabb.min().x, aabb.min().y, aabb.max().z));
					mesh.positions.push_back(vec3(aabb.min().x, aabb.min().y, aabb.min().z));
					mesh.positions.push_back(vec3(aabb.min().x, aabb.max().y, aabb.min().z));
					mesh.positions.push_back(vec3(aabb.min().x, aabb.max().y, aabb.max().z));
					mesh.positions.push_back(vec3(aabb.max().x, aabb.min().y, aabb.min().z));
					mesh.positions.push_back(vec3(aabb.max().x, aabb.max().y, aabb.min().z));
					mesh.positions.push_back(vec3(aabb.max().x, aabb.min().y, aabb.max().z));
					mesh.positions.push_back(vec3(aabb.max().x, aabb.max().y, aabb.max().z));



					GLuint indices[] = {
						3, 2, 2, 5, 5,
						7, 7, 3, 3, 0,
						0, 6, 6, 4, 4,
						1, 1, 0, 0, 3,
						5, 4, 2, 1, 7,
						6
					};

					mesh.indices = vector<GLuint>(begin(indices), end(indices));
					mesh.colors = vector<vec4>(8, vec4(BLACK));
					mesh.primitiveType = GL_LINES;
					return new ProvidedMesh(mesh);
				}
			}
		}

	}
}