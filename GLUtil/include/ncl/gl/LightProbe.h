#pragma once

#include <functional>
#include <variant>
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
#include "shader_binding.h"
#include "../geom/sphere_bv.h"
#include "../geom/OBB.h"

namespace ncl {
	namespace gl {

		class Probe : public Drawable {
		public:
			using Sphere = geom::bvol::Sphere;
			using Box = geom::bvol::OBB;
			using Geometry = std::variant<Sphere, Box>;

			Probe() = default;


			Probe(Scene* scene, glm::vec3 loc, float alpha, GLsizei width, GLsizei height, std::string fragmentShader = scene_capture_phong_frag_shader, int id = 0);

			Probe(Scene* scene, glm::vec3 loc, float alpha, FrameBuffer::Config config, std::string fragmentShader = scene_capture_phong_frag_shader, int id = 0);

			Probe(const Probe&) = delete;

			Probe(Probe&& source) noexcept;

			Probe& operator=(const Probe&) = delete;

			Probe& operator=(Probe&& source) noexcept;

			friend void transfer(Probe& source, Probe& dest);

			void capture(std::function<void()> scene);

			void draw(Shader& shader) override;

			void render(int index = 0);

			GLuint texture(int index = 0);

			int id() {
				return _id;
			}

			inline int captured() {
				return framebuffer.numAttachments();
			}

		protected:
			void initShaders(std::string fragmentShader);

			static FrameBuffer::Config defaultConfig(GLsizei w, GLsizei h);

		private:
			Scene* scene;
			glm::vec3 location;
			float alpha;
			glm::mat4 views[6];
			Shader sceneCapture;
			Shader renderShader;
			FrameBuffer framebuffer;
			Cube cube;
			gl::Sphere sphere;
			Geometry geometry;
			int _id = 0;
		};
		
		Probe::Probe(Scene* scene, glm::vec3 loc, float alpha, GLsizei width, GLsizei height, std::string fragmentShader, int id)
			: Probe(scene, loc, alpha, defaultConfig(width, height), fragmentShader, id){}

		Probe::Probe(Scene* scene, glm::vec3 loc, float alpha, FrameBuffer::Config config, std::string fragmentShader, int id)
			: scene{ scene }
			, location{ loc }
			, alpha{ alpha }
			, framebuffer{ config }
			, _id{ id }{

			views[0] = glm::lookAt(loc, loc + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[1] = glm::lookAt(loc, loc + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[2] = glm::lookAt(loc, loc + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			views[3] = glm::lookAt(loc, loc + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			views[4] = glm::lookAt(loc, loc + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			views[5] = glm::lookAt(loc, loc + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			cube = Cube{ 1, WHITE, {}, false };
			cube.defautMaterial(false);

			sphere = gl::Sphere{ 0.1, 10, 10, YELLOW };

			initShaders(fragmentShader);
		}

		Probe::Probe(Probe&& source) noexcept {
			transfer(source, *this);
		}

		Probe& Probe::operator=(Probe&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		void transfer(Probe& source, Probe& dest) {
			dest.location = source.location;
			dest.alpha = source.alpha;
			
			for (int i = 0; i < 6; i++) {
				dest.views[i] = source.views[i];
				source.views[i] = glm::mat4{ 1 };
			}

			dest.sceneCapture = std::move(source.sceneCapture);
			dest.renderShader = std::move(source.renderShader);
			dest.framebuffer = std::move(source.framebuffer);
			dest.sphere = std::move(source.sphere);

			dest.cube = std::move(source.cube);
			dest.geometry = source.geometry;

			dest.scene = source.scene;

			source.scene = nullptr;
		}

		void Probe::initShaders(std::string fragmentShader) {
			sceneCapture.load({ GL_VERTEX_SHADER, scene_capture_vert_shader, "scene_capture.vert" });
			sceneCapture.load({ GL_GEOMETRY_SHADER, scene_capture_geom_shader, "scene_capture.geom" });
			sceneCapture.load({ GL_FRAGMENT_SHADER, fragmentShader, "scene_capture.frag" });
			sceneCapture.createAndLinkProgram();

			renderShader.load({ GL_VERTEX_SHADER, probe_render_vert_shader, "probe_render.vert" });
			renderShader.load({ GL_FRAGMENT_SHADER, probe_render_frag_shader, "probe_render.vert" });
			renderShader.createAndLinkProgram();
		}

		FrameBuffer::Config Probe::defaultConfig(GLsizei w, GLsizei h) {
			auto config = FrameBuffer::Config{ w, h };
			config.fboTarget = GL_FRAMEBUFFER;
			config.depthAndStencil = true;
			config.depthTest = true;
			config.stencilTest = false;
			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_NEAREST;
			attachment.minfilter = GL_NEAREST;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_CUBE_MAP;
			attachment.internalFmt = GL_RGB8;
			attachment.fmt = GL_RGB;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			attachment.texLevel = 0;
			config.attachments.push_back(attachment);

			return config;
		}

		void Probe::capture(std::function<void()> renderScene) {
			using namespace std;
			auto projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
			framebuffer.use([&] {
				sceneCapture.use([&] {
					//glEnable(GL_DEPTH_TEST);
					//glDepthFunc(GL_LESS);
					sceneCapture.sendUniform3fv("camPos", 1, glm::value_ptr(location));
					sceneCapture.sendUniformMatrix4fv("M", 1, false, glm::value_ptr(glm::mat4{ 1 }));
					sceneCapture.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(projection));
					sceneCapture.sendUniformMatrix4fv("views", 6, false, glm::value_ptr(views[0]));
					renderScene();
				});
			});
		}

		void Probe::draw(Shader& shader) {
			auto model = glm::translate(glm::mat4{ 1 }, location);
			shader.sendUniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(model));
			sphere.draw(shader);
		}

		inline void Probe::render(int index) {
			renderShader([&] {
				auto& camera = scene->activeCamera();
				glDepthFunc(GL_LEQUAL);
				//glm::mat4 MVP = camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix()));
				glBindTextureUnit(0, framebuffer.texture(index));
				//s.sendUniformMatrix4fv("MVP", 1, GL_FALSE, value_ptr(MVP));
				send("probe_loc", location);
				send(camera);
				shade(&cube);
				glDepthFunc(GL_LESS);
			});
		}


		inline GLuint Probe::texture(int index) {
			return framebuffer.texture(index);
		}
	}

}