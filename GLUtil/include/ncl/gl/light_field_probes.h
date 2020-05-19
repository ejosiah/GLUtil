#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>
#include <gl/gl_core_4_5.h>
#include "textures.h"
#include "Shader.h"
#include "shaders.h"
#include "FrameBuffer.h"
#include "LightProbe.h"

namespace ncl {
	namespace gl {

		template<size_t N>
		struct ProbeGrid {
			Texture texture;
			glm::vec4 size;
			glm::vec4 invSize;
			glm::vec4 readMultiplyFirst = glm::vec4(2);
			glm::vec4 readAddSecond = glm::vec4(-1);
			bool notNull;
			const size_t dim = N;
		};

		using ProbeGrid2DArray = ProbeGrid<2>;
		using probeGridCubeArray = ProbeGrid<2>;

		struct LightFieldSurface {
			ProbeGrid2DArray radianceProbeGrid;
			ProbeGrid2DArray normalProbeGrid;
			ProbeGrid2DArray distanceProbeGrid;
			ProbeGrid2DArray lowResolutionDistanceProbeGrid;
			glm::ivec3 probeCounts;
			glm::vec3 probeStartPosition;
			glm::vec3 probeStep;
			int lowResolutionDownsampleFactor;
			probeGridCubeArray irradianceProbeGrid;
			probeGridCubeArray meanDistProbeGrid;
		};

		class LightFieldProbes : public Drawable{
		public:
			struct Config {
				glm::vec3 probeCount = { 2, 2, 2 };
				glm::vec3 probeStep = { 1, 1, 1 };
				glm::vec3 startProbeLocation = { 0, 0, 0 };
				GLsizei resolution = 1024;
				struct {
					int numSamples = 2048;
					float lobeSize = 1.0f;
					GLsizei resolution = 128;
				} meanDistance;
				struct {
					int numSamples = 128;
					float lobeSize = 1.0f;
					GLsizei resolution = 128;
				} irradiance;
				std::string captureFragmentShader;
			};

			LightFieldProbes() = default;

			LightFieldProbes(Config config, Scene* scene);

			LightFieldProbes(const LightFieldProbes&) = default;

			LightFieldProbes(LightFieldProbes&& source) noexcept;

			LightFieldProbes& operator=(const LightFieldProbes&) = delete;

			LightFieldProbes& operator=(LightFieldProbes&& source) noexcept;

			friend void transfer(LightFieldProbes& source, LightFieldProbes& dest);

			void update(Config config);

			void capture(std::function<void()> scene);

			void renderProbe(int index = 0);

			void renderOctahedrals(int attachment = 0);

			void renderOctahedral(int index = 0);

			void renderIrradiance(int index = 0);

			void renderMeadDistance(int index = 0);

			inline void init() {
				initProbes();
				initOctahedral();
				initIrradiaceProbeGrid();
				initMeanDistanceProbeGrid();
			}
		protected:

			void initShaders();

			void initProbes();

			void initOctahedral();

			void initIrradiaceProbeGrid();

			void initMeanDistanceProbeGrid();

			void generateOctahedrals();

			void generateIrradiaceGrid();

			void generateMeanDistanceGrid();

			void draw(Shader& shader) override;

			gl::FrameBuffer::Config probeConfig();

			gl::FrameBuffer::Config octahedralConfig();

			gl::FrameBuffer::Config irradianceProbeGridConfig();

			gl::FrameBuffer::Config meanDistanceProbeGridConfig();

		private:
			Config config;
			std::vector<Probe> probes;
			FrameBuffer octahedral;
			FrameBuffer lowResolutionDistanceProbeGrid;
			int lowResolutionDownsampleFactor;
			FrameBuffer irradianceProbeGrid;
			FrameBuffer meanDistanceProbeGrid;
			Shader octahedralShader;
			Shader irradianceShader;
			Shader octahedralRenderShader;
			Shader irradianceRenderShader;
			Scene* scene;
			Cube cube;
			ProvidedMesh quad;
			LightFieldSurface lightSurface;
		};

		static glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 1000.0f);
		static glm::mat4 views[6]{
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};

		LightFieldProbes::LightFieldProbes(Config config, Scene* scene)
			:config{ config }
			, scene{ scene }
		{
			cube = Cube{ 1, WHITE, {}, false };
			cube.defautMaterial(false);
			quad = ProvidedMesh{ screnSpaceQuad() };
			quad.defautMaterial(false);
			initShaders();
		}

		LightFieldProbes::LightFieldProbes(LightFieldProbes&& source)  noexcept {
			transfer(source, *this);
		}

		LightFieldProbes& LightFieldProbes::operator=(LightFieldProbes&& source)  noexcept{
			transfer(source, *this);
			return *this;
		}

		void transfer(LightFieldProbes& source, LightFieldProbes& dest) {

			if (&source == &dest) return;

			dest.config = source.config;
			dest.probes = std::move(source.probes);
			dest.octahedral = std::move(source.octahedral);
			dest.lowResolutionDistanceProbeGrid = std::move(source.lowResolutionDistanceProbeGrid);
			dest.lowResolutionDownsampleFactor = source.lowResolutionDownsampleFactor;
			dest.irradianceProbeGrid = std::move(source.irradianceProbeGrid);
			dest.meanDistanceProbeGrid = std::move(source.meanDistanceProbeGrid);
			dest.octahedralShader = std::move(source.octahedralShader);
			dest.irradianceShader = std::move(source.irradianceShader);
			dest.octahedralRenderShader = std::move(source.octahedralRenderShader);
			dest.irradianceRenderShader = std::move(source.irradianceRenderShader);
			dest.scene = source.scene;
			dest.cube = std::move(source.cube);
			dest.quad = std::move(source.quad);

			source.scene = nullptr;
		}

		void LightFieldProbes::update(Config config) {
			this->config = config;
		}

		void LightFieldProbes::initShaders() {
			octahedralShader.load({ GL_VERTEX_SHADER, octahedral_vert_shader, "octahedral.vert" });
			octahedralShader.load({ GL_FRAGMENT_SHADER, octahedral_frag_shader, "octahedral.frag" });
			octahedralShader.createAndLinkProgram();

			octahedralRenderShader.load({ GL_VERTEX_SHADER, screen_vert_shader, "octahedral_render.vert" });
			octahedralRenderShader.load({ GL_GEOMETRY_SHADER , octahedral_render_geom_shader, "octahedral_render.geom" });
			octahedralRenderShader.load({ GL_FRAGMENT_SHADER, octahedral_render_frag_shader, "octahedral_render.frag" });
			octahedralRenderShader.createAndLinkProgram();

			irradianceShader.load({ GL_VERTEX_SHADER, lfp_prefilter_vert_shader, "irradiance.vert" });
			irradianceShader.load({ GL_GEOMETRY_SHADER, lfp_prefilter_geom_shader, "irradiance.geom" });
			irradianceShader.load({ GL_FRAGMENT_SHADER, lfp_prefilter_frag_shader, "irradiance.frag" });
			irradianceShader.createAndLinkProgram();


			irradianceRenderShader.load({ GL_VERTEX_SHADER, lfp_irradiance_render_vert_shader, "irradiance_render.vert" });
			irradianceRenderShader.load({ GL_FRAGMENT_SHADER, lfp_irradiance_render_frag_shader, "irradiance_render.frag" });
			irradianceRenderShader.createAndLinkProgram();

		}

		void LightFieldProbes::initProbes() {
			auto pConfig = probeConfig();
			for (int z = 0; z < config.probeCount.z; z++) {
				for (int y = 0; y < config.probeCount.y; y++) {
					for (int x = 0; x < config.probeCount.x; x++) {
						glm::vec3 location = glm::vec3(x, y, z) * config.probeStep + config.startProbeLocation;
						probes.emplace_back(scene, location, 1.0f, pConfig, config.captureFragmentShader);
					}
				}
			}
		}

		void LightFieldProbes::initOctahedral() {
			auto config = octahedralConfig();
			octahedral = FrameBuffer{ config };
		}

		void LightFieldProbes::initIrradiaceProbeGrid() {
			irradianceProbeGrid = FrameBuffer{ irradianceProbeGridConfig() };
		}


		void LightFieldProbes::initMeanDistanceProbeGrid() {
			meanDistanceProbeGrid = FrameBuffer{ meanDistanceProbeGridConfig() };
		}

		void LightFieldProbes::capture(std::function<void()> scene) {
			for (auto& probe : probes) {
				probe.capture([&] {
					scene();
				});
			}
			generateOctahedrals();
			generateIrradiaceGrid();
			generateMeanDistanceGrid();
		}

		void LightFieldProbes::generateOctahedrals() {
			octahedral.use([&] {
				octahedralShader([&] {
					for (int layer = 0; layer < probes.size(); layer++) {
						octahedral.attachTextureFor(layer);
						auto& probe = probes[layer];
						glBindTextureUnit(0, probes[layer].texture(0));
						glBindTextureUnit(1, probes[layer].texture(1));
						glBindTextureUnit(2, probes[layer].texture(2));
						shade(quad);
					}
					});
				});
		}

		void LightFieldProbes::generateIrradiaceGrid() {
			irradianceProbeGrid.use([&] {
				irradianceShader([&] {
					send("numSamples", config.irradiance.numSamples);
					send("lobeSize", config.irradiance.lobeSize);
					send("irradiance", true);	// TODO try subroutine
					irradianceShader.sendUniformMatrix4fv("views", 6, false, glm::value_ptr(views[0]));
					irradianceShader.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(projection));
					for (int layer = 0; layer < probes.size(); layer++) {
						send("layer", layer);
						//	irradianceProbeGrid.attachTextureFor(layer);
						auto& probe = probes[layer];
						glBindTextureUnit(0, probes[layer].texture(0));
						shade(cube);
					}
					});
				});
		}

		void LightFieldProbes::generateMeanDistanceGrid() {
			meanDistanceProbeGrid.use([&] {
				irradianceShader([&] {
					send("numSamples", config.meanDistance.numSamples);
					send("lobeSize", config.meanDistance.lobeSize);
					send("irradiance", false);	// TODO try subroutine
					irradianceShader.sendUniformMatrix4fv("views", 6, false, glm::value_ptr(views[0]));
					irradianceShader.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(projection));
					for (int layer = 0; layer < probes.size(); layer++) {
						send("layer", layer);
						//	irradianceProbeGrid.attachTextureFor(layer);
						auto& probe = probes[layer];
						glBindTextureUnit(0, probes[layer].texture(2));
						shade(cube);
					}
					});
				});
		}

		void LightFieldProbes::renderProbe(int index) {
			assert(index > 0 || index < probes.size());
			probes.at(index).render();
		}

		void LightFieldProbes::renderOctahedrals(int attachment) {
			octahedralRenderShader([&] {
				send("isDistance", attachment == 2);
				send("numLayers", int(probes.size()));
				glBindTextureUnit(0, octahedral.texture(attachment));
				glBindTextureUnit(1, irradianceProbeGrid.texture());
				glBindTextureUnit(2, meanDistanceProbeGrid.texture());
				shade(quad);
			});
		}

		void LightFieldProbes::renderOctahedral(int index) {

		}

		void LightFieldProbes::renderIrradiance(int index) {

		}

		void LightFieldProbes::renderMeadDistance(int index) {

		}

		void LightFieldProbes::draw(Shader& shader) {
			for (auto& probe : probes) {
				probe.draw(shader);
			}
		}

		gl::FrameBuffer::Config LightFieldProbes::probeConfig() {
			auto pConfig = FrameBuffer::Config{ config.resolution, config.resolution };
			pConfig.fboTarget = GL_FRAMEBUFFER;
			pConfig.depthAndStencil = true;
			pConfig.depthTest = true;
			pConfig.stencilTest = false;

			for (int i = 0; i < 3; i++) {
				auto attachment = FrameBuffer::Attachment{};
				attachment.magFilter = GL_NEAREST;
				attachment.minfilter = GL_NEAREST;
				attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
				attachment.texTarget = GL_TEXTURE_CUBE_MAP;
				attachment.internalFmt = GL_RGBA32F;
				attachment.fmt = GL_RGBA;
				attachment.type = GL_FLOAT;
				attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
				attachment.texLevel = 0;
				pConfig.attachments.push_back(attachment);
			}


			return pConfig;
		}

		gl::FrameBuffer::Config LightFieldProbes::octahedralConfig() {
			//auto oConfig = FrameBuffer::Config{ config.resolution, config.resolution };
			//oConfig.fboTarget = GL_FRAMEBUFFER;
			//oConfig.depthAndStencil = false;
			//oConfig.depthTest = false;
			//oConfig.stencilTest = false;

			//for (int i = 0; i < 3; i++) {
			//	auto attachment = FrameBuffer::Attachment{};
			//	attachment.magFilter = GL_NEAREST;
			//	attachment.minfilter = GL_NEAREST;
			//	attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			//	attachment.texTarget = GL_TEXTURE_2D_ARRAY;
			//	attachment.internalFmt = GL_R11F_G11F_B10F;
			//	attachment.fmt = GL_RGBA;
			//	attachment.type = GL_FLOAT;
			//	attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
			//	attachment.texLevel = 0;
			//	attachment.numLayers = probes.size();
			//	oConfig.attachments.push_back(attachment);
			//}

			//oConfig.attachments[2].internalFmt = GL_RG16F;
			//oConfig.attachments[2].fmt = GL_RG;

			//return oConfig;
			auto config = FrameBuffer::Config{ 1024, 1024 };
			config.fboTarget = GL_FRAMEBUFFER;
			config.depthAndStencil = false;
			config.depthTest = false;
			config.stencilTest = false;

			for (int i = 0; i < 3; i++) {
				auto attachment = FrameBuffer::Attachment{};
				attachment.magFilter = GL_NEAREST;
				attachment.minfilter = GL_NEAREST;
				attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
				attachment.texTarget = GL_TEXTURE_2D_ARRAY;
				attachment.internalFmt = GL_R11F_G11F_B10F;
				attachment.fmt = GL_RGBA;
				attachment.type = GL_FLOAT;
				attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
				attachment.texLevel = 0;
				attachment.numLayers = probes.size();
				config.attachments.push_back(attachment);
			}

			config.attachments[2].internalFmt = GL_RG16F;
			config.attachments[2].fmt = GL_RG;

			return config;
		}

		gl::FrameBuffer::Config LightFieldProbes::irradianceProbeGridConfig() {
			auto fConfig = FrameBuffer::Config{ config.irradiance.resolution, config.irradiance.resolution };
			fConfig.fboTarget = GL_FRAMEBUFFER;
			fConfig.depthAndStencil = false;
			fConfig.depthTest = false;
			fConfig.stencilTest = false;

			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_NEAREST;
			attachment.minfilter = GL_NEAREST;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_CUBE_MAP_ARRAY;
			attachment.internalFmt = GL_R11F_G11F_B10F;
			attachment.fmt = GL_RGBA;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			attachment.texLevel = 0;
			attachment.numLayers = probes.size();
			fConfig.attachments.push_back(attachment);

			return fConfig;
		}

		gl::FrameBuffer::Config LightFieldProbes::meanDistanceProbeGridConfig() {
			auto fConfig = FrameBuffer::Config{ config.meanDistance.resolution, config.meanDistance.resolution };
			fConfig.fboTarget = GL_FRAMEBUFFER;
			fConfig.depthAndStencil = false;
			fConfig.depthTest = false;
			fConfig.stencilTest = false;

			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_NEAREST;
			attachment.minfilter = GL_NEAREST;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_CUBE_MAP_ARRAY;
			attachment.internalFmt = GL_RG32F;
			attachment.fmt = GL_RG;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			attachment.texLevel = 0;
			attachment.numLayers = probes.size();
			fConfig.attachments.push_back(attachment);

			return fConfig;
		}
	}	
}