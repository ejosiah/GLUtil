#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <gl/gl_core_4_5.h>
#include "Shader.h"
#include "Sphere.h"
#include "Cube.h"
#include "Drawable.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include "SkyBox.h"
#include "shaders.h"

namespace ncl {
	namespace gl {
		class LightProbe : public Drawable {
		public:
			LightProbe() = default;

			LightProbe(Scene* scene, glm::vec3 loc, float alpha, GLsizei width, GLsizei height);

			LightProbe(const LightProbe&) = delete;

			LightProbe(LightProbe&& source) noexcept;

			LightProbe& operator=(const LightProbe&) = delete;

			LightProbe& operator=(LightProbe&& source) noexcept;

			friend void transfer(LightProbe& source, LightProbe& dest);

			void capture(std::function<void()> scene);

			void draw(Shader& shader) override;

			void renderPreFiltered(CameraController& camera, float roughness = 0);

			void renderIrradiance(CameraController& camera);

			void render();

		protected:
			void initShaders();

			void initFrameBuffers();

		private:
			Scene* scene;
			glm::vec3 location;
			float alpha;
			int width;
			int height;
			glm::mat4 views[6];
			Shader sceneCapture;
			SkyBox captured;
			FrameBuffer framebuffer;
		};
		
		LightProbe::LightProbe(Scene* scene, glm::vec3 loc, float alpha, GLsizei width, GLsizei height)
			: scene{ scene }
			,location{ loc }
			, alpha{ alpha }
			, width{ width }
			, height{ height }{

			views[0] = glm::lookAt(loc, loc + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[1] = glm::lookAt(loc, loc + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[2] = glm::lookAt(loc, loc + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			views[3] = glm::lookAt(loc, loc + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			views[4] = glm::lookAt(loc, loc + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[5] = glm::lookAt(loc, loc + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			initShaders();
			initFrameBuffers();
		}

		LightProbe::LightProbe(LightProbe&& source) noexcept {
			transfer(source, *this);
		}

		LightProbe& LightProbe::operator=(LightProbe&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		void transfer(LightProbe& source, LightProbe& dest) {
			dest.location = source.location;
			dest.alpha = source.alpha;
			dest.width = source.width;
			dest.height = source.height;
			
			for (int i = 0; i < 6; i++) {
				dest.views[i] = source.views[i];
				source.views[i] = glm::mat4{ 1 };
			}

			dest.sceneCapture = std::move(source.sceneCapture);
			dest.captured = std::move(source.captured);
			dest.framebuffer = std::move(source.framebuffer);
			dest.scene = source.scene;

			source.scene = nullptr;
		}

		void LightProbe::initShaders() {
			sceneCapture.load({ GL_VERTEX_SHADER, scene_capture_vert_shader, "scene_capture.vert" });
			sceneCapture.load({ GL_GEOMETRY_SHADER, scene_capture_geom_shader, "scene_capture.geom" });
			sceneCapture.load({ GL_FRAGMENT_SHADER, scene_capture_phong_frag_shader, "scene_capture.frag" });
			sceneCapture.createAndLinkProgram();
		}

		void LightProbe::initFrameBuffers() {
			auto config = FrameBuffer::Config{ width, height };
			config.magFilter = GL_LINEAR;
			config.minfilter = GL_LINEAR_MIPMAP_LINEAR;
			config.wrap_t = config.wrap_s = GL_CLAMP_TO_EDGE;
			config.fboTarget = GL_FRAMEBUFFER;
			config.texTarget = GL_TEXTURE_2D;
			config.internalFmt = GL_RGB16;
			config.fmt = GL_RGB;
			config.type = GL_FLOAT;
			config.attachment = GL_COLOR_ATTACHMENT0;
			config.depthAndStencil = true;

			framebuffer = FrameBuffer{ config };
		}

		void LightProbe::capture(std::function<void()> renderScene) {
			auto projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
			framebuffer.use([&] {
				sceneCapture.use([&] {
					sceneCapture.sendUniform3fv("camPos", 1, glm::value_ptr(location));
					sceneCapture.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(projection));
					sceneCapture.sendUniformMatrix4fv("views", 6, false, glm::value_ptr(views[0]));
					renderScene();
				});
			});
			captured = SkyBox{ scene, framebuffer.texture()};
		}

		void LightProbe::draw(Shader& shader) {

		}

		inline void LightProbe::render() {
			captured.render();
		}

		void LightProbe::renderPreFiltered(CameraController& camera, float roughness) {

		}

		void LightProbe::renderIrradiance(CameraController& camera) {

		}
	}

}