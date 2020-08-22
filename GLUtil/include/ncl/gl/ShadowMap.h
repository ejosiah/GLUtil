#pragma once


#include <String>
#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "shaders.h"
#include "shader_binding.h"
#include "FrameBuffer.h"
#include <functional>

namespace ncl {
	namespace gl {
		
		class ShadowMap {
		public:
			virtual void capture(std::function<void()> scene) = 0;

			virtual void render() = 0;

			virtual GLuint texture() = 0;

			virtual void sendTo(Shader& shader) = 0;
		};

		class DirectionalShadowMap : public ShadowMap{
		public:
			struct LightView {
				glm::mat4 view;
				glm::mat4 projection;
				bool perspective = false;
			};

			DirectionalShadowMap() = default;

			DirectionalShadowMap(GLuint textureUnit, LightView lv, GLsizei width, GLsizei height);

			DirectionalShadowMap(DirectionalShadowMap&& source) noexcept;

			DirectionalShadowMap(const DirectionalShadowMap& source) = delete;

			DirectionalShadowMap& operator=(DirectionalShadowMap&& source) noexcept;

			DirectionalShadowMap& operator=(const DirectionalShadowMap&& source) = delete;

			void capture(std::function<void()> scene) override;

			void update(glm::mat4 view);

			void render() override;

			virtual void sendTo(Shader& shader) override;

			const LightView& getLightview() const {
				return lightView;
			}

			glm::mat4 lightViewMatrix() const {
				return lightView.projection * lightView.view;
			}

			std::tuple<float, float> nearFarPerspective(glm::mat4 projection);

			GLuint texture() {
				return frameBuffer.texture();
			}

			friend void transfer(DirectionalShadowMap& source, DirectionalShadowMap& dest);

		private:
			FrameBuffer frameBuffer;
			Shader shadowShader;
			Shader debugShader;
			LightView lightView;
			ProvidedMesh quad;
			GLuint textureUnit;
		};

		class OminiDirectionalShadowMap : public ShadowMap {
		public:
			struct LightView {
				glm::vec3 position;
				glm::mat4 views[6];
				glm::mat4 projection;
			};

			OminiDirectionalShadowMap() = default;

			OminiDirectionalShadowMap(GLuint textureUnit, glm::vec3 position, GLsizei width, GLsizei height, float near = 1.0f, float far = 25.0f);

			OminiDirectionalShadowMap(OminiDirectionalShadowMap&& source) noexcept;

			OminiDirectionalShadowMap(const OminiDirectionalShadowMap& source) = delete;

			OminiDirectionalShadowMap& operator=(OminiDirectionalShadowMap&& source) noexcept;

			OminiDirectionalShadowMap& operator=(const OminiDirectionalShadowMap&& source) = delete;

			void capture(std::function<void()> scene) override;

			void update(glm::vec3 position);

			void render() override;

			inline GLuint texture() override {
				return frameBuffer.texture();
			}

			void sendTo(Shader& shader) override;

			friend void transfer(OminiDirectionalShadowMap& source, OminiDirectionalShadowMap& dest);

		private:
			FrameBuffer frameBuffer;
			Shader shadowShader;
			Shader debugShader;
			Cube cube;
			GLuint textureUnit;
			LightView lightView;
			float farPlane, nearPlane;
		};

		OminiDirectionalShadowMap::OminiDirectionalShadowMap(GLuint textureUnit, glm::vec3 position, GLsizei width, GLsizei height, float znear, float zfar)
			: textureUnit{ textureUnit }
			, nearPlane{ znear }
			, farPlane{ zfar } {
			auto config = FrameBuffer::Config{ width, height };
			auto attachment = FrameBuffer::Attachment{};
			config.depthTest = true;
			config.stencilTest = false;
			config.read = config.write = false;
			config.read = config.write = false;
			config.depthAndStencil = false;
			attachment.texTarget = GL_TEXTURE_CUBE_MAP;
			attachment.magFilter = attachment.minfilter = GL_NEAREST;
			attachment.wrap_s = attachment.wrap_t = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.internalFmt = attachment.fmt = GL_DEPTH_COMPONENT;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_DEPTH_ATTACHMENT;
			config.attachments.push_back(attachment);
			frameBuffer = FrameBuffer{ config };

			shadowShader.load({ GL_VERTEX_SHADER, point_shadow_map_vert_shader, "point_shadow_map.vert" });
			shadowShader.load({ GL_GEOMETRY_SHADER, point_shadow_map_geom_shader, "point_shadow_map.geom" });
			shadowShader.load({ GL_FRAGMENT_SHADER, point_shadow_map_frag_shader, "point_shadow_map.frag" });
			shadowShader.createAndLinkProgram();

			debugShader.load({ GL_VERTEX_SHADER, point_shadow_map_render_vert_shader, "point_shadow_map.vert" });
			debugShader.load({ GL_FRAGMENT_SHADER, point_shadow_map_render_frag_shader, "point_shadow_map.frag" });
			debugShader.createAndLinkProgram();

			cube = Cube{ 1 };
			
			lightView.projection = glm::perspective(glm::half_pi<float>(), float(width) / float(height), nearPlane, farPlane);
			update(position);
		}

		OminiDirectionalShadowMap::OminiDirectionalShadowMap(OminiDirectionalShadowMap&& source) noexcept {
			transfer(source, *this);
		}

		OminiDirectionalShadowMap& OminiDirectionalShadowMap::operator=(OminiDirectionalShadowMap&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		void OminiDirectionalShadowMap::capture(std::function<void()> scene) {
			frameBuffer.use([&]() {
				shadowShader.use([&]() {
					shadowShader.sendUniform1f("nearPlane", nearPlane);
					shadowShader.sendUniform1f("farPlane", farPlane);
					shadowShader.sendUniform3fv("lightPos", 1, glm::value_ptr(lightView.position));
					shadowShader.sendUniformMatrix4fv("M", 1, false, glm::value_ptr(glm::mat4{ 1 }));
					shadowShader.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(lightView.projection));
					for (int i = 0; i < 6; i++) {
						shadowShader.sendUniformMatrix4fv("view[" + std::to_string(i) + "]", 1, false, glm::value_ptr(lightView.views[i]));
					}
					//shadowShader.sendUniformMatrix4fv("view", 6, false, glm::value_ptr(lightView.views[0]));
					scene();
					});
				});
		}

		void OminiDirectionalShadowMap::update(glm::vec3 position) {
			lightView.position = position;
			lightView.views[0] = glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			lightView.views[1] = glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			lightView.views[2] = glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			lightView.views[3] = glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			lightView.views[4] = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			lightView.views[5] = glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		}

		void OminiDirectionalShadowMap::render() {
			debugShader([&] {
				glBindTextureUnit(0, frameBuffer.texture());
				cube.draw(debugShader);
			});

		}

		void OminiDirectionalShadowMap::sendTo(Shader& shader) {
			glBindTextureUnit(textureUnit, frameBuffer.texture());
			shader.sendUniform1f("farPlane", farPlane);
			shader.sendUniform1i("shadowType", 1);
		}

		void transfer(OminiDirectionalShadowMap& source, OminiDirectionalShadowMap& dest) {
			dest.frameBuffer = std::move(source.frameBuffer);
			dest.shadowShader = std::move(source.shadowShader);
			dest.debugShader = std::move(source.debugShader);
			dest.lightView = source.lightView;
			dest.cube = std::move(source.cube);
			dest.textureUnit = source.textureUnit;
			dest.nearPlane = source.nearPlane;
			dest.farPlane = source.farPlane;
		}
	}
}

#include "detail/ShadowMap.inl"