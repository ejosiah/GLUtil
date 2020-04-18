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

		static inline Texture2D* defaultText() {
			return new CheckerTexture(1, "diffuse", WHITE, GRAY);
		}

		class Floor : public Drawable {
		public:
			Floor(int grids, int length, const Scene& scene)
				:grids{ grids }, length{ length }, scene{ scene }{}

			void init(Texture2D* text = defaultText()) {
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
				Material& mat = plane->material();
				mat.ambient = { 0.0f,0.0f,0.0f,1.0f };
				mat.diffuse = { 0.55f,0.55f,0.55f,1.0f };
				mat.specular = { 0.70f,0.70f,0.70f,1.0f };
				mat.shininess = 32.0f;
				mat.diffuseMat = 1;
			}
			
			virtual void draw(Shader& shader) override {
				send(texture.get());
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