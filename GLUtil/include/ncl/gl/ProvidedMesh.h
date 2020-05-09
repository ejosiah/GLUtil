#pragma once
#include "Shape.h"
#include "WithTriangleAdjacency.h"

namespace ncl {
	namespace gl {
		class ProvidedMesh : public Shape, public WithTriangleAdjacency {
		public:
			ProvidedMesh() = default;

			explicit ProvidedMesh(Mesh mesh, bool adjacency = false, unsigned int instanceCount = 1) :ProvidedMesh(std::vector<Mesh>(1, mesh), adjacency, instanceCount) {
			}

			explicit ProvidedMesh(std::vector<Mesh> meshes, bool adjacency = false, unsigned int instanceCount = 1)
				:Shape(preprocess(meshes, adjacency), false, instanceCount) {
			}

			explicit ProvidedMesh(Shape& shape, GLenum primitiveType, const color& color = glm::vec4(1), unsigned int instanceCount = 1)	// TODO add bitfields of what to copy
				:Shape(copyMesh(shape, primitiveType, color), false, instanceCount) {

			}

			ProvidedMesh(const ProvidedMesh&) = delete;

			ProvidedMesh(ProvidedMesh&& source) noexcept 
				: Shape(dynamic_cast<Shape&&>(source))  {

			}

			virtual ~ProvidedMesh() = default;

			ProvidedMesh& operator=(const ProvidedMesh&) = delete;
			
			ProvidedMesh& operator=(ProvidedMesh&& source) noexcept {
				Shape::transfer(dynamic_cast<Shape&>(source), dynamic_cast<Shape&>(*this));
				return *this;
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