#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include "common.h"
#include "Scene.h"
#include "Image.h"
#include "shader_binding.h"


namespace ncl {
	namespace gl {
		class SkyBox {
		public:
			SkyBox() = default;

			SkyBox(Scene* scene, GLuint buffer, GLuint unit = 0, int size = 1) 
				:scene(scene), buffer(buffer), unit(unit) {
				cube = new Cube(size, WHITE, {}, DONT_CULL_BACK_FACE);
			}

			SkyBox(const SkyBox&) = delete;

			SkyBox(SkyBox&& source) noexcept;

			SkyBox& operator=(const SkyBox&) = delete;

			SkyBox& operator=(SkyBox&& source) noexcept;

			friend void transfer(SkyBox& source, SkyBox& dest) {
				dest.scene = source.scene;
				dest.buffer = source.buffer;
				dest.unit = source.unit;
				dest.cube = source.cube;

				source.scene = nullptr;
				source.buffer = 0;
				source.cube = nullptr;
			}

			void render();

			void bind() {
				glBindTextureUnit(unit, buffer);
			}

			static SkyBox* create(std::vector<std::string> faces, GLuint textureUnit, Scene& scene, int size = 1);


			static SkyBox* create(Shader& shader, GLuint textureUnit, Scene& scene, Texture2D& texture, GLsizei width = 512, GLsizei height = 512);

			static SkyBox* preFilter(Shader& shader, GLuint textureUnit, Scene& scene, Texture2D& texture, int lod = 5, GLsizei width = 128, GLsizei height = 128);

			Cube* cube;

		public:
			Scene* scene;
			GLuint buffer;
			GLuint unit;
			
		};

		SkyBox::SkyBox(SkyBox&& source) noexcept {
			transfer(source, *this);
		}

		SkyBox& SkyBox::operator=(SkyBox&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		SkyBox* SkyBox::create(std::vector<std::string> faces, GLuint textureUnit, Scene& scene, int size) {
			GLuint skyBoxId;
			glGenTextures(1, &skyBoxId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);

			for (int i = 0; i < 6; i++) {
				auto name = faces[i];
				auto path = name;
				auto img = Image(path, IL_ORIGIN_UPPER_LEFT);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
			}


			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			return new SkyBox(&scene, skyBoxId, textureUnit, size);
		}

		SkyBox* SkyBox::create(Shader& shader, GLuint textureUnit, Scene& scene, Texture2D& texture, GLsizei width, GLsizei height) {
			GLuint fbo, rbo;
			glViewport(0, 0, width, height);
			glGenFramebuffers(1, &fbo); 
			glGenRenderbuffers(1, &rbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

			GLuint skyBoxId;
			glGenTextures(1, &skyBoxId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);
			for (int i = 0; i < 6; i++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			GlmCam cam;
			cam.projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
			glm::mat4 views[6]{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

			};
			
			auto cube = new Cube(1, WHITE, {}, DONT_CULL_BACK_FACE);
			shader([&]() {
				send(&texture);
				for (int i = 0; i < 6; i++) {
					cam.view = views[i];
					send(cam);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyBoxId, 0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					shade(cube);
				}
			});
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			delete cube;
			glDeleteRenderbuffers(1, &rbo);
			glDeleteFramebuffers(1, &fbo);

			glViewport(0, 0, scene.width(), scene.height());

			return new SkyBox(&scene, skyBoxId, textureUnit, 1);
		}

		SkyBox* SkyBox::preFilter(Shader& shader, GLuint textureUnit, Scene& scene, Texture2D& texture, int lod, GLsizei width, GLsizei height) {
			GLuint fbo, rbo;
			glViewport(0, 0, width, height);
			glGenFramebuffers(1, &fbo);
			glGenRenderbuffers(1, &rbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

			GLuint skyBoxId;
			glGenTextures(1, &skyBoxId);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxId);
			for (int i = 0; i < 6; i++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			GlmCam cam;
			cam.projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
			glm::mat4 views[6]{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

			};

			auto cube = new Cube(1, WHITE, {}, DONT_CULL_BACK_FACE);
			for (unsigned int level = 0; level < lod; level++) {
				// reisze framebuffer according to mip-level size.
				unsigned int w = width * std::pow(0.5, level);
				unsigned int h = height * std::pow(0.5, level);
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
				glViewport(0, 0, w, h);

				float roughness = (float)level / (float)(lod - 1);
				shader([&]() {
					send("roughness", roughness);
					send(&texture);
					for (int i = 0; i < 6; i++) {
						cam.view = views[i];
						send(cam);
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyBoxId, level);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						shade(cube);
					}
					});
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			delete cube;
			glDeleteRenderbuffers(1, &rbo);
			glDeleteFramebuffers(1, &fbo);

			glViewport(0, 0, scene.width(), scene.height());

			return new SkyBox(&scene, skyBoxId, textureUnit, 1);
		}

		void SkyBox::render() {
			scene->shader("skybox")([&](Shader& s) {
				auto& camera = scene->activeCamera();
				glDepthFunc(GL_LEQUAL);
				//glm::mat4 MVP = camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix()));
				glBindTextureUnit(unit, buffer);
				//s.sendUniformMatrix4fv("MVP", 1, GL_FALSE, value_ptr(MVP));
				send(camera);
				shade(cube);
				glDepthFunc(GL_LESS);
			});
		}
	}
}