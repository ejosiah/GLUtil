#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "logger.h"
#include "mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace ncl {
	namespace gl {
		class WithTangent {
		public:
			void addTangent(Mesh& mesh, bool useSizeMappedUV) {
				using namespace std;
				using namespace glm;

				int numTriangles = mesh.indices.empty() ? mesh.positions.size() / 3 : mesh.indices.size() / 3;

				for (int i = 0; i < numTriangles; i += 3) {
					auto tri = next(mesh, useSizeMappedUV, i);
					auto q1 = tri.v1 - tri.v0;
					auto q2 = tri.v2 - tri.v0;
					auto s = tri.u1 - tri.u0;
					auto t = tri.u2 - tri.u0;

					glm::mat3x2 Q = glm::mat3x2(q1.x, q2.x, q1.y, q2.y, q1.z, q2.z);
					glm::mat2 UV = glm::mat2(s.x, t.x, s.y, t.y);

					mat3x2 TB = inverse(UV) * Q;
					
					mesh.tangents.push_back(normalize(row(TB, 0)));
					mesh.bitangents.push_back(normalize(row(TB, 1)));
				}
			}

		private:
			ncl::Logger logger = ncl::Logger::get("WithTangent");

			struct Triangle{
				glm::vec3 v0, v1, v2;
				glm::vec2 u0, u1, u2;
			};

			Triangle next(Mesh& m, bool useSizeMappedUV, int i) {
				// TODO check primitive type and code appropiately
				int uvId = useSizeMappedUV ? 1 : 0;
				if (m.indices.empty()) {
					auto v0 = m.positions[i * 3];
					auto v1 = m.positions[i * 3 + 1];
					auto v2 = m.positions[i * 3 + 2];
					auto u0 = m.uvs[uvId][i * 3];
					auto u1 = m.uvs[uvId][i * 3 + 1];
					auto u2 = m.uvs[uvId][i * 3 + 2];

					return { v0, v1, v2, u0, u1, u2 };
				}
				else {
					auto v0 = m.positions[m.indices[i * 3]];
					auto v1 = m.positions[m.indices[i * 3 + 1]];
					auto v2 = m.positions[m.indices[i * 3 + 2]];
					auto u0 = m.uvs[uvId][m.indices[i * 3]];
					auto u1 = m.uvs[uvId][m.indices[i * 3 + 1]];
					auto u2 = m.uvs[uvId][m.indices[i * 3 + 2]];
					return { v0, v1, v2, u0, u1, u2 };
				}
			}
		};
	}
}