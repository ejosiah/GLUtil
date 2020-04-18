#pragma once
#include "../GLUtil/include/ncl/gl/Drawable.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/mesh.h"
#include "../GLUtil/include/ncl/gl/ProvidedMesh.h"
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include <memory>

namespace ncl {
	namespace gl {

		class Floor : public Drawable {
		public:
			Floor(int grids, int length, const Scene& scene)
				:grids{ grids }, length{ length }, scene{ scene }{}

			void init(Texture2D* text = new CheckerTexture()) {
				texture = std::unique_ptr<Texture2D>{ text };
				float inner[2]{ grids, grids };
				float outer[4]{ grids, grids, grids, grids };
				glPatchParameteri(GL_PATCH_VERTICES, 4);
				glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, inner);
				glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outer);

				float h = length * 0.5;
				Mesh mesh;
				mesh.positions.emplace_back(-h, 0, h);
				mesh.positions.emplace_back(h, 0, h);
				mesh.positions.emplace_back(h, 0, -h);
				mesh.positions.emplace_back(-h, 0, -h);
				mesh.primitiveType = GL_PATCHES;
				plane = std::make_unique<ProvidedMesh>(mesh);
			}

			virtual void draw(Shader& shader) override {
				shade(plane.get());
			}

		private:
			std::unique_ptr<Texture2D> texture;
			std::unique_ptr<ProvidedMesh> plane;
			const Scene& scene;
			int grids;
			int length;
		};
	};
}