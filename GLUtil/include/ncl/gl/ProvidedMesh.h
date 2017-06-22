#pragma once
#include "Shape.h"

namespace ncl {
	namespace gl {
		class ProvidedMesh : public Shape {
		public:
			ProvidedMesh(Mesh& mesh) :ProvidedMesh(std::vector<Mesh>(1, mesh)) {
			}

			ProvidedMesh(std::vector<Mesh>& meshes):Shape(meshes) {
			}

			ProvidedMesh(Shape& shape, GLenum primitiveType, const color& color = glm::vec4(1))	// TODO add bitfields of what to copy
				:Shape(copyMesh(shape, primitiveType, color)) {

			}

		private:
			std::vector<Mesh> copyMesh(const Shape& shape, GLenum primitiveType, const color& color) {
				Mesh mesh;
				mesh.primitiveType = primitiveType;
				mesh.material.diffuse = color;
				shape.get<glm::vec3>(Position, [&](glm::vec3* v) {
					int size = shape.numVertices(0);
					for (int i = 0; i < size; i++) {
						glm::vec3 vCopy = *v;
						mesh.positions.push_back(vCopy);
						v++;
					}
				});

				return std::vector<Mesh>(1, mesh);
			}
		};
	}
}