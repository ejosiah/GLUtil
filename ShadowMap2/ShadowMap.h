#pragma once

#include <gl/gl_core_4_5.h>
#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Shader.h"
#include "../GLUtil/include/ncl/gl/shaders.h"
#include "../GLUtil/include/ncl/gl/FrameBuffer.h"
#include "../GLUtil/include/ncl/gl/ProvidedMesh.h"
#include "../GLUtil/include/ncl/gl/primitives.h"


namespace ncl {
	namespace gl {
		class ShadowMap0{
		public:
			struct LightView {
				glm::mat4 view;
				glm::mat4 projection;
				bool perspective = false;
			};

			ShadowMap0() = default;

			ShadowMap0(GLuint textureUnit, LightView lv, GLsizei width, GLsizei height) 
				:lightView{ lv }
				, textureUnit{ textureUnit }{
				auto config = FrameBuffer::Config{ width, height };
				config.magFilter = config.minfilter = GL_LINEAR;
				config.wrap_s = config.wrap_t = GL_CLAMP_TO_EDGE;
				config.internalFmt = GL_DEPTH_COMPONENT32;
				config.fmt = GL_DEPTH_COMPONENT;
				config.type = GL_FLOAT;
				config.read = config.write = false;
				config.depthAndStencil = false;
				config.attachment = GL_DEPTH_ATTACHMENT;
				config.depthTest = true;
				config.stencilTest = true;
				config.texTarget = GL_TEXTURE_2D;
				auto extra = []() {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(2.0f, 4.0f);
				};
				frameBuffer = FrameBuffer{ config, extra };

				shadowShader.loadFromFile("shaders/shadow_map.vert");
				shadowShader.loadFromFile("shaders/shadow_map.frag");
				shadowShader.createAndLinkProgram();

				debugShader.loadFromFile("shaders/shadow_map_render.vert");
				debugShader.loadFromFile("shaders/shadow_map_render.frag");
				debugShader.createAndLinkProgram();

				quad = ProvidedMesh{ screnSpaceQuad() };
				quad.defautMaterial(false);

				scaleAndBias = glm::translate(glm::mat4{ 1 }, { 0.5, 0.5, 0.5 }) * glm::scale(glm::mat4{ 1 }, { 0.5, 0.5, 0.5 });
			}

			ShadowMap0(ShadowMap0&& source) noexcept {
				transfer(source, *this);
			}

			ShadowMap0(const ShadowMap0& source) = delete;

			ShadowMap0& operator=(ShadowMap0&& source) noexcept {
				transfer(source, *this);
				return *this;
			}

			ShadowMap0& operator=(const ShadowMap0&& source) = delete;

			void capture(std::function<void()> scene) {
				frameBuffer.use([&]() {
					shadowShader([&]() {
						auto lightSpaceXform = lightView.projection * lightView.view;
						//shadowShader.sendUniformMatrix4fv("M", 1, false, glm::value_ptr(glm::mat4{ 1 }));
						shadowShader.sendUniformMatrix4fv("lightSpaceView", 1, false, glm::value_ptr(lightSpaceXform));
						scene();
						});
					});
			}

			void update(glm::mat4 view) {
				lightView.view = view;
			}

			void render() {
				debugShader([&] {
					glBindTextureUnit(0, frameBuffer.texture());
					quad.draw(debugShader);
				});
			}

			virtual void sendTo(Shader& shader) {
				auto lightSpaceView = scaleAndBias * lightView.projection * lightView.view;
				shadowShader.sendUniformMatrix4fv("lightSpaceView", 1, false, glm::value_ptr(lightSpaceView));
				glBindTextureUnit(textureUnit, frameBuffer.texture());
			}

			glm::mat4 lightSpaceMatrix() {
				return scaleAndBias * lightView.projection * lightView.view;
			}

			const LightView& getLightview() const {
				return lightView;
			}

			glm::mat4 lightViewMatrix() const {
				return lightView.projection * lightView.view;
			}


			GLuint texture() {
				return frameBuffer.texture();
			}

			static inline void transfer(ShadowMap0& source, ShadowMap0& dest) {
				dest.frameBuffer = std::move(source.frameBuffer);
				dest.shadowShader = std::move(source.shadowShader);
				dest.debugShader = std::move(source.debugShader);
				dest.lightView = source.lightView;
				dest.quad = std::move(source.quad);
				dest.textureUnit = source.textureUnit;
				dest.scaleAndBias = source.scaleAndBias;
			}

		private:
			FrameBuffer frameBuffer;
			Shader shadowShader;
			Shader debugShader;
			LightView lightView;
			glm::mat4 scaleAndBias;
			ProvidedMesh quad;
			GLuint textureUnit;
		};
	}
}