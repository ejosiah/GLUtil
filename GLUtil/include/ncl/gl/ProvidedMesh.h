#pragma once
#include "Shape.h"
#include "WithTriangleAdjacency.h"

namespace ncl {
	namespace gl {
		class ProvidedMesh : public Shape, public WithTriangleAdjacency {
		public:
			ProvidedMesh(Mesh& mesh, bool adjacency = false) :ProvidedMesh(std::vector<Mesh>(1, mesh), adjacency) {
			}

			ProvidedMesh(std::vector<Mesh>& meshes, bool adjacency = false):Shape(preprocess(meshes, adjacency)) {
			}

			ProvidedMesh(Shape& shape, GLenum primitiveType, const color& color = glm::vec4(1))	// TODO add bitfields of what to copy
				:Shape(copyMesh(shape, primitiveType, color)) {

			}

		private:
			Logger logger = Logger::get("Provided Mesh");
			std::vector<Mesh> copyMesh(const Shape& shape, GLenum primitiveType, const color& color) {
				Mesh mesh;
				mesh.primitiveType = primitiveType;
				mesh.material.diffuse = color;
				shape.get<glm::vec3>(0, Position, [&](glm::vec3* v) {
					int size = shape.numVertices(0);
					for (int i = 0; i < size; i++) {
						glm::vec3 vCopy = *v;
						mesh.positions.push_back(vCopy);
						v++;
					}
				});

				return std::vector<Mesh>(1, mesh);
			}
			std::vector<Mesh>& preprocess(std::vector<Mesh>& meshes, bool adjacency) {
				if (adjacency) {
					logger.info("added adjacency data to mesh");
					for (Mesh& mesh : meshes) {
						mesh.indices = addAdjacency(mesh.indices);
					}
				}
				return meshes;
			}
		};


	}
}