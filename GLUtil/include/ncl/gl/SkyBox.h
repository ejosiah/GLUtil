#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "Scene.h"
#include "Image.h"

namespace ncl {
	namespace gl {
		class SkyBox {
		public:
			SkyBox(Scene& scene, GLuint buffer, GLuint unit, int size) 
				:scene(scene), buffer(buffer), unit(unit) {
				cube = new Cube(size, 10, WHITE, false);
			}

			void render();

			void bind() {
				glBindTextureUnit(unit, buffer);
			}

			static SkyBox* create(std::vector<std::string> faces, GLuint textureUnit, Scene& scene, int size);

			Cube* cube;

		private:
			Scene & scene;
			GLuint buffer;
			GLuint unit;
			
		};

		SkyBox* SkyBox::create(std::vector<std::string> faces, GLuint textureUnit, Scene& scene, int size) {
			GLuint skyBoxId;
			glGenTextures(1, &skyBoxId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);

			for (int i = 0; i < 6; i++) {
				auto name = faces[i];
				auto path = name;
				auto img = Image(path, IL_ORIGIN_UPPER_LEFT);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
			}


			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			return new SkyBox(scene, skyBoxId, textureUnit, size);
		}

		void SkyBox::render() {
			scene.shader("skybox")([&](Shader& s) {
				auto& camera = scene.activeCamera();
				glDepthFunc(GL_LEQUAL);
				glm::mat4 MVP = camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix()));
				glBindTextureUnit(unit, buffer);
				Shader::boundShader->sendUniform1i("id", 0);
				s.sendUniformMatrix4fv("MVP", 1, GL_FALSE, value_ptr(MVP));
				cube->draw(s);
				glDepthFunc(GL_LESS);
			});
		}
	}
}