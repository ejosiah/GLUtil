#pragma once

#include "../primitives.h"

namespace ncl {
	namespace gl {
		DirectionalShadowMap::DirectionalShadowMap(GLuint textureUnit, LightView lv, GLsizei width, GLsizei height)
			:lightView{ lv }
			, textureUnit{ textureUnit } {
			auto config = FrameBuffer::Config{ width, height };
			config.read = config.write = false;
			config.depthAndStencil = false;
			config.depthTest = true;
			config.stencilTest = false;
			config.read = config.write = false;
			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = attachment.minfilter = GL_NEAREST;
			attachment.wrap_s = attachment.wrap_t = GL_CLAMP_TO_BORDER;
			attachment.internalFmt = attachment.fmt = GL_DEPTH_COMPONENT;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_DEPTH_ATTACHMENT;
			config.attachments.push_back(attachment);
			frameBuffer = FrameBuffer{ config };
			//	initRaw(width, height);

			shadowShader.load({ GL_VERTEX_SHADER, shadow_map_vert_shader, "shadow_map.vert" });
			shadowShader.load({ GL_FRAGMENT_SHADER, shadow_map_frag_shader, "shadow_map.frag" });
			shadowShader.createAndLinkProgram();

			debugShader.load({ GL_VERTEX_SHADER, shadow_map_render_vert_shader, "shadow_map_render.vert" });
			debugShader.load({ GL_FRAGMENT_SHADER, shadow_map_render_frag_shader, "shadow_map_render.frag" });
			debugShader.createAndLinkProgram();

			quad = ProvidedMesh{ screnSpaceQuad() };
		}

		DirectionalShadowMap::DirectionalShadowMap(DirectionalShadowMap&& source) noexcept {
			transfer(source, *this);
		}

		DirectionalShadowMap& DirectionalShadowMap::operator=(DirectionalShadowMap&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		void DirectionalShadowMap::capture(std::function<void()> scene) {
			glCullFace(GL_FRONT);
			frameBuffer.use([&]() {
				shadowShader([&]() {
					auto lightSpaceXform = lightView.projection * lightView.view;
					shadowShader.sendUniformMatrix4fv("M", 1, false, glm::value_ptr(glm::mat4{ 1 }));
					shadowShader.sendUniformMatrix4fv("lightSpaceView", 1, false, glm::value_ptr(lightSpaceXform));
					scene();
					});
				});
			glCullFace(GL_BACK);
		}

		void DirectionalShadowMap::update(glm::mat4 view) {
			lightView.view = view;
		}

		void DirectionalShadowMap::render() {
			debugShader([&] {
				glBindTextureUnit(0, frameBuffer.texture());
				//	glBindTextureUnit(0, depthMap);
				debugShader.sendUniform1i("perspective", lightView.perspective);
				if (lightView.perspective) {
					auto [nearPlane, farPlane] = nearFarPerspective(lightView.projection);
					debugShader.sendUniform1f("near_plane", nearPlane);
					debugShader.sendUniform1f("far_plane", farPlane);
				}

				//gl::shade(quad);
				quad.draw(debugShader);

				});
		}

		void DirectionalShadowMap::sendTo(Shader& shader) {
			auto [nearPlane, farPlane] = nearFarPerspective(lightView.projection);
			shader.sendUniform1f("near_plane", nearPlane);
			shader.sendUniform1f("far_plane", farPlane);
			shader.sendUniformMatrix4fv("lightSpaceView", 1, false, glm::value_ptr(lightViewMatrix()));
			glBindTextureUnit(textureUnit, frameBuffer.texture());
			shader.sendUniform1i("shadowType", 0);
		}

		inline void transfer(DirectionalShadowMap& source, DirectionalShadowMap& dest) {
			dest.frameBuffer = std::move(source.frameBuffer);
			dest.shadowShader = std::move(source.shadowShader);
			dest.debugShader = std::move(source.debugShader);
			dest.lightView = source.lightView;
			dest.quad = std::move(source.quad);
			dest.textureUnit = source.textureUnit;
		}

		std::tuple<float, float> DirectionalShadowMap::nearFarPerspective(glm::mat4 projection) {

			auto C = projection[2][2];
			auto D = projection[3][2];

			float nearP = D / (C - 1.0f);
			float farP = D / (C + 1.0f);

			return std::make_tuple(nearP, farP);
		}
	}
}