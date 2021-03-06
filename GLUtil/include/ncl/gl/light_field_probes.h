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
#include "common.h"

namespace ncl {
	namespace gl {

		enum class Lfp : int {
			Radiance, Normal, Distance
		};

		static int toInt(Lfp lfp) {
			return static_cast<int>(lfp);
		}

		template<size_t N>
		struct ProbeGrid {
			Texture texture;
			glm::vec4 size;
			glm::vec4 invSize;
			glm::vec4 readMultiplyFirst = glm::vec4(1);
			glm::vec4 readAddSecond = glm::vec4(0);
			bool notNull;
			const size_t dim = N;
		};

		using ProbeGrid2DArray = ProbeGrid<3>;
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
				GLsizei octResolution = 512;
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
				int lowResolutionDownsampleFactor = 16;
				GLint textureBindOffset = 0;
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

			void renderOctahedrals(Lfp attachment = Lfp::Radiance);

			void renderOctahedral(Lfp attachment = Lfp::Radiance, int index = 0);

			void renderLowResDistanceProbes();

			void renderLowResDistanceProbe(int index = 0);

			void renderIrradiance(int index = 0);

			void renderMeanDistance(int index = 0);

			GLsizei resolution() {
				return config.octResolution;
			}

			int numProbes() {
				return config.probeCount.x * config.probeCount.y * config.probeCount.z;
			}

			void sendTo(Shader& shader);

			inline void init() {
				initProbes();
				initOctahedral();
				initLowResDistanceProbeGrid();
				initIrradiaceProbeGrid();
				initMeanDistanceProbeGrid();
			}
		protected:

			void initShaders();

			void initProbes();

			void initOctahedral();

			void initLowResDistanceProbeGrid();

			void initIrradiaceProbeGrid();

			void initMeanDistanceProbeGrid();

			void generateOctahedrals(std::function<void()> scene);

			void convoluteOctahedrals();

			void generateOctahedralMipMap();

			void generateLowResDistanceProbe();

			void generateIrradiaceGrid();

			void generateMeanDistanceGrid();

			void draw(Shader& shader) override;

			gl::FrameBuffer::Config probeConfig();

			gl::FrameBuffer::Config octahedralConfig();

			gl::FrameBuffer::Config irradianceProbeGridConfig();

			gl::FrameBuffer::Config meanDistanceProbeGridConfig();

			struct ProbeInfo {
				int index;
				glm::vec3 location;
			};

		public:
			Config config;
			std::vector<Probe> probes;	// we don't need to keep this around
			std::vector<ProbeInfo> probeInfos;	// we don't need to keep this around
			FrameBuffer octahedral;
			FrameBuffer lowResolutionDistanceProbeGrid;
			int lowResolutionDownsampleFactor = 1;
			FrameBuffer irradianceProbeGrid;
			FrameBuffer meanDistanceProbeGrid;
			Shader octahedralShader;
			Shader LowResDistanceShader;
			Shader irradianceShader;
			Shader octahedralRenderShader;
			Shader irradianceRenderShader;
			Shader convolutionShader;
			Shader copy2dArrayShader;
			Shader renderProbeShader;
			Scene* scene;
			Cube cube;
			Sphere sphere;
			ProvidedMesh quad;
			LightFieldSurface lightFieldSurface;
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
			sphere = Sphere{ 0.1, 10, 10, YELLOW };
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
			dest.probeInfos = std::move(source.probeInfos);
			dest.octahedral = std::move(source.lowResolutionDistanceProbeGrid);
			dest.lowResolutionDistanceProbeGrid = std::move(source.octahedral);
			dest.lowResolutionDistanceProbeGrid = std::move(source.lowResolutionDistanceProbeGrid);
			dest.lowResolutionDownsampleFactor = source.lowResolutionDownsampleFactor;
			dest.irradianceProbeGrid = std::move(source.irradianceProbeGrid);
			dest.meanDistanceProbeGrid = std::move(source.meanDistanceProbeGrid);
			dest.octahedralShader = std::move(source.octahedralShader);
			dest.LowResDistanceShader = std::move(source.LowResDistanceShader);
			dest.irradianceShader = std::move(source.irradianceShader);
			dest.octahedralRenderShader = std::move(source.octahedralRenderShader);
			dest.irradianceRenderShader = std::move(source.irradianceRenderShader);
			dest.convolutionShader = std::move(source.convolutionShader);
			dest.renderProbeShader = std::move(source.renderProbeShader);
			dest.copy2dArrayShader = std::move(source.copy2dArrayShader);
			dest.scene = source.scene;
			dest.cube = std::move(source.cube);
			dest.quad = std::move(source.quad);
			dest.sphere = std::move(source.sphere);

			source.scene = nullptr;
		}

		void LightFieldProbes::update(Config config) {
			this->config = config;
		}

		void LightFieldProbes::initShaders() {
			octahedralShader.load({ GL_VERTEX_SHADER, octahedral_vert_shader, "octahedral.vert" });
			octahedralShader.load({ GL_FRAGMENT_SHADER, octahedral_frag_shader, "octahedral.frag" });
			octahedralShader.createAndLinkProgram();

			LowResDistanceShader.load({ GL_VERTEX_SHADER, octahedral_vert_shader, "low_res_distance.vert" });
			LowResDistanceShader.load({ GL_FRAGMENT_SHADER, octahedral_low_res_distance_frag_shader, "low_res_distance.frag" });
			LowResDistanceShader.createAndLinkProgram();

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

			convolutionShader.load({ GL_VERTEX_SHADER, octahedral_vert_shader, "octahedral_convolution.vert" });
			convolutionShader.load({ GL_FRAGMENT_SHADER, octahedral_convolution_frag_shader, "octahedral_convolution.frag" });
			convolutionShader.createAndLinkProgram();

			renderProbeShader.load({ GL_VERTEX_SHADER , skybox_vert_shader, "probe_render.vert" });
			renderProbeShader.load({ GL_FRAGMENT_SHADER, octahedral_skybox_frag_shader, "probe_render.frag" });
			renderProbeShader.createAndLinkProgram();

			copy2dArrayShader.load({ GL_VERTEX_SHADER, screen_vert_shader, "copy_arry2d.vert" });
			copy2dArrayShader.load({ GL_FRAGMENT_SHADER, copy_array2d_frag_shader, "copy_arry2d.vert" });
			copy2dArrayShader.createAndLinkProgram();

		}

		void LightFieldProbes::initProbes() {
			auto pConfig = probeConfig();
			for (int z = 0; z < config.probeCount.z; z++) {
				for (int y = 0; y < config.probeCount.y; y++) {
					for (int x = 0; x < config.probeCount.x; x++) {
						int index = (z * config.probeCount.x * config.probeCount.y) + (y * config.probeCount.x) + x;
						glm::vec3 location = glm::vec3(x, y, z) * config.probeStep + config.startProbeLocation;
					//	probes.emplace_back(scene, location, 1.0f, pConfig, config.captureFragmentShader);
						probeInfos.push_back(ProbeInfo{ index, location });
					}
				}
			}
		}

		void LightFieldProbes::initOctahedral() {
			auto config = octahedralConfig();
			octahedral = FrameBuffer{ config };
		}

		void LightFieldProbes::initLowResDistanceProbeGrid() {
			auto dConfig = octahedralConfig();
			dConfig.width = dConfig.height = config.octResolution * (1 / float(config.lowResolutionDownsampleFactor));
			auto attachment = *(dConfig.attachments.end() - 1);
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			dConfig.attachments.clear();
			dConfig.attachments.push_back(attachment);

			lowResolutionDistanceProbeGrid = FrameBuffer{ dConfig };
		}

		void LightFieldProbes::initIrradiaceProbeGrid() {
			irradianceProbeGrid = FrameBuffer{ irradianceProbeGridConfig() };
		}


		void LightFieldProbes::initMeanDistanceProbeGrid() {
			meanDistanceProbeGrid = FrameBuffer{ meanDistanceProbeGridConfig() };
		}

		void LightFieldProbes::capture(std::function<void()> scene) {
			//for (auto& probe : probes) {
			//	probe.capture([&] {
			//		scene();
			//	});
			//}
			generateOctahedrals(scene);
			convoluteOctahedrals();
			generateLowResDistanceProbe();
			generateIrradiaceGrid();
			generateMeanDistanceGrid();

			lightFieldSurface.radianceProbeGrid.texture = { config.textureBindOffset, octahedral.texture(toInt(Lfp::Radiance)) };
			lightFieldSurface.radianceProbeGrid.size = glm::vec4{ config.octResolution, config.octResolution, probeInfos.size(), 1 };
			lightFieldSurface.radianceProbeGrid.invSize = 1.0f / lightFieldSurface.radianceProbeGrid.size;

			lightFieldSurface.normalProbeGrid.texture = { config.textureBindOffset + 1, octahedral.texture(toInt(Lfp::Normal)) };
			lightFieldSurface.normalProbeGrid.size = glm::vec4{ config.octResolution, config.octResolution, probeInfos.size(), 1 };
			lightFieldSurface.normalProbeGrid.invSize = 1.0f / lightFieldSurface.normalProbeGrid.size;
			lightFieldSurface.normalProbeGrid.readMultiplyFirst = glm::vec4{ 2 };
			lightFieldSurface.normalProbeGrid.readAddSecond = glm::vec4{ -1 };

			lightFieldSurface.distanceProbeGrid.texture = { config.textureBindOffset + 2, octahedral.texture(toInt(Lfp::Distance)) };
			lightFieldSurface.distanceProbeGrid.size = glm::vec4{ config.octResolution, config.octResolution, probeInfos.size(), 1 };
			lightFieldSurface.distanceProbeGrid.invSize = 1.0f / lightFieldSurface.distanceProbeGrid.size;

			auto res = config.octResolution * (1 / float(config.lowResolutionDownsampleFactor));
			lightFieldSurface.lowResolutionDistanceProbeGrid.texture = { config.textureBindOffset + 3, lowResolutionDistanceProbeGrid.texture() };
			lightFieldSurface.lowResolutionDistanceProbeGrid.size = glm::vec4{ res , res, probeInfos.size(), 1 };
			lightFieldSurface.lowResolutionDistanceProbeGrid.invSize = 1.0f / lightFieldSurface.lowResolutionDistanceProbeGrid.size;

			lightFieldSurface.irradianceProbeGrid.texture = { config.textureBindOffset + 4, irradianceProbeGrid.texture() };
			lightFieldSurface.irradianceProbeGrid.size = glm::vec4{ config.irradiance.resolution, config.irradiance.resolution, 0, 0 };
			lightFieldSurface.irradianceProbeGrid.invSize = 1.0f / lightFieldSurface.irradianceProbeGrid.size;

			lightFieldSurface.meanDistProbeGrid.texture = { config.textureBindOffset + 5, meanDistanceProbeGrid.texture() };
			lightFieldSurface.meanDistProbeGrid.size = glm::vec4{ config.meanDistance.resolution, config.meanDistance.resolution, 0, 0 };
			lightFieldSurface.meanDistProbeGrid.invSize = 1.0f / lightFieldSurface.meanDistProbeGrid.size;

			
			lightFieldSurface.probeCounts = config.probeCount;
			lightFieldSurface.probeStartPosition = config.startProbeLocation;
			lightFieldSurface.probeStep = config.probeStep;
			lightFieldSurface.lowResolutionDownsampleFactor = config.lowResolutionDownsampleFactor;

		}

		void LightFieldProbes::generateOctahedrals(std::function<void()> scene) {
			octahedral.use([&] {
				octahedralShader([&] {
					for (int layer = 0; layer < probeInfos.size(); layer++) {
						octahedral.attachTextureFor(layer);
						auto pConfig = probeConfig();
						std::future<Probe> futureProbe = this->scene->renderOffScreen<Probe>([&]() {
							auto probe = Probe{this->scene, probeInfos[layer].location, 1.0f, pConfig, config.captureFragmentShader };
							probe.capture([&] {
								scene();
							});
							return probe;
						});

						auto probe = futureProbe.get();
						glBindTextureUnit(0, probe.texture(0));
						glBindTextureUnit(1, probe.texture(1));
						glBindTextureUnit(2, probe.texture(2));
						quad.draw(octahedralShader);
					}
					});
				});
	//		generateOctahedralMipMap();
		}

		void LightFieldProbes::convoluteOctahedrals() {
			auto resolution = this->config.octResolution;
			auto config = FrameBuffer::Config{ resolution, resolution };
			config.fboTarget = GL_FRAMEBUFFER;
			config.depthAndStencil = false;
			config.depthTest = false;
			config.stencilTest = false;
			auto attachment = FrameBuffer::Attachment{};
			attachment.magFilter = GL_LINEAR;
			attachment.minfilter = GL_LINEAR_MIPMAP_LINEAR;
			attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
			attachment.texTarget = GL_TEXTURE_2D_ARRAY;
			attachment.internalFmt = GL_R11F_G11F_B10F;
			attachment.fmt = GL_RGB;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			attachment.numLayers = numProbes();
			attachment.mipMap = true;
			attachment.texLevel = 0;
			config.attachments.push_back(attachment);

			auto convolution = FrameBuffer{ config };

			quad.defautMaterial(false);
			convolution.use([&] {
				convolutionShader([&] {
					for (int layer = 0; layer < numProbes(); layer++) {
						const int lod = 6;
						for (int level = 0; level < lod; level++) {
							unsigned int w = resolution * std::pow(0.5, level);
							unsigned int h = resolution * std::pow(0.5, level);
							convolution.attachTextureFor(layer, level);
							glViewport(0, 0, w, h);

							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
							glBindTextureUnit(0, octahedral.texture(toInt(Lfp::Radiance)));
							float roughness = (float)level / (float)(lod - 1);
							convolutionShader.sendUniform1i("layer", layer);
							convolutionShader.sendUniform1f("roughness", roughness);
							convolutionShader.sendUniform1f("resolution", resolution);
							quad.draw(convolutionShader);
						}
					}
				});
			});

			octahedral.use([&] {
				copy2dArrayShader([&] {
					for (int layer = 0; layer < numProbes(); layer++) {
						const int lod = 6;
						for (int level = 0; level < lod; level++) {
							unsigned int w = resolution * std::pow(0.5, level);
							unsigned int h = resolution * std::pow(0.5, level);
							octahedral.attachTextureFor(layer, level, { 0 });
							glViewport(0, 0, w, h);

							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
							glBindTextureUnit(0, convolution.texture());
							copy2dArrayShader.sendUniform1i("layer", layer);
							quad.draw(copy2dArrayShader);
						}
					}
				});
			});
		}

		void LightFieldProbes::generateOctahedralMipMap() {
			octahedral.use([&] {
				convolutionShader([&] {
					for (int layer = 0; layer < 64; layer++) {
						const int lod = 6;
						for (int level = 0; level < lod; level++) {
							unsigned int w = 512 * std::pow(0.5, level);
							unsigned int h = 512 * std::pow(0.5, level);
							octahedral.attachTextureFor(layer, level);
							glViewport(0, 0, w, h);

							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
							glBindTextureUnit(0, octahedral.texture(toInt(Lfp::Radiance)));
							float roughness = (float)level / (float)(lod - 1);
							convolutionShader.sendUniform1i("layer", layer);
							convolutionShader.sendUniform1f("roughness", roughness);
							convolutionShader.sendUniform1f("resolution", 512);
							quad.draw(convolutionShader);
						}
					}
				});
			});
		}

		void LightFieldProbes::generateLowResDistanceProbe() {
			lowResolutionDistanceProbeGrid.use([&] {
				LowResDistanceShader([&] {
					for (int layer = 0; layer < probeInfos.size(); layer++) {
						lowResolutionDistanceProbeGrid.attachTextureFor(layer);
						//auto& probe = probes[layer];
						//glBindTextureUnit(0, probes[layer].texture(2));
						LowResDistanceShader.sendUniform1i("layer", layer);
						glBindTextureUnit(0, octahedral.texture(toInt(Lfp::Distance)));
						shade(quad);
					}
				});
			});
		}

		void LightFieldProbes::generateIrradiaceGrid() {
			irradianceProbeGrid.use([&] {
				irradianceShader([&] {
					irradianceShader.sendUniform1i("numSamples", config.irradiance.numSamples);
					irradianceShader.sendUniform1f("lobeSize", config.irradiance.lobeSize);
					irradianceShader.sendBool("irradiance", true);	// TODO try subroutine
					irradianceShader.sendUniformMatrix4fv("views", 6, false, glm::value_ptr(views[0]));
					irradianceShader.sendUniformMatrix4fv("projection", 1, false, glm::value_ptr(projection));
					for (int layer = 0; layer < probeInfos.size(); layer++) {
						//	irradianceProbeGrid.attachTextureFor(layer);
						//auto& probe = probes[layer];
						//glBindTextureUnit(0, probes[layer].texture(0));

						irradianceShader.sendUniform1i("layer", layer);
						glBindTextureUnit(0, octahedral.texture(toInt(Lfp::Radiance)));
						cube.draw(irradianceShader);
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
					for (int layer = 0; layer < probeInfos.size(); layer++) {
						send("layer", layer); // TODO use irradianceShader explicitly
						//irradianceProbeGrid.attachTextureFor(layer);
					//	auto& probe = probes[layer];
					//	glBindTextureUnit(0, probes[layer].texture(2));
						glBindTextureUnit(0, octahedral.texture(toInt(Lfp::Distance)));
						shade(cube);	// TODO use irradianceShader explicitly
					}
					});
				});
		}

		void LightFieldProbes::renderProbe(int index) {
			assert(index > 0 || index < probeInfos.size());
			//probes.at(index).render();
			renderProbeShader([&] {
				clearBindings();
				glDepthFunc(GL_LEQUAL);
				renderProbeShader.sendComputed(scene->activeCamera(), glm::mat4{ 1 });
				renderProbeShader.sendUniform1i("numLayers", probeInfos.size());
				renderProbeShader.sendUniform1i("layer", index);
				renderProbeShader.sendBool("isDistance", false);
				renderProbeShader.sendBool("isDistanceSqrd", false);
				glBindTextureUnit(0, octahedral.texture(0));
				cube.draw(renderProbeShader);
				glDepthFunc(GL_LESS);
			});
		}

		void LightFieldProbes::renderOctahedrals(Lfp attachment) {
			octahedralRenderShader([&] {
				send("isDistance", static_cast<int>(attachment) == 2);
				send("numLayers", int(probeInfos.size()));
				send("renderAll", true);
				glBindTextureUnit(0, octahedral.texture(static_cast<int>(attachment)));
				glBindTextureUnit(1, irradianceProbeGrid.texture());
				glBindTextureUnit(2, meanDistanceProbeGrid.texture());
				shade(quad);
			});
		}

		void LightFieldProbes::renderOctahedral(Lfp attachment, int index) {
			octahedralRenderShader([&] {
				send("isDistance", static_cast<int>(attachment) == 2);
				send("numLayers", int(probeInfos.size()));
				send("renderAll", false);
				send("uLayer", index);
				glBindTextureUnit(0, octahedral.texture(static_cast<int>(attachment)));
				glBindTextureUnit(1, irradianceProbeGrid.texture());
				glBindTextureUnit(2, meanDistanceProbeGrid.texture());
				shade(quad);
			});
		}
		

		void LightFieldProbes::renderLowResDistanceProbes() {
			octahedralRenderShader([&] {
				send("isDistance", true);
				send("numLayers", int(probeInfos.size()));
				send("renderAll", true);
				glBindTextureUnit(0, lowResolutionDistanceProbeGrid.texture());
				shade(quad);
			});
		}

		void LightFieldProbes::renderLowResDistanceProbe(int index) {
			octahedralRenderShader([&] {
				send("isDistance", true);
				send("numLayers", int(probeInfos.size()));
				send("renderAll", false);
				send("uLayer", index);
				glBindTextureUnit(0, lowResolutionDistanceProbeGrid.texture());
				shade(quad);
			});
		}

		void LightFieldProbes::renderIrradiance(int index) {
			glDepthFunc(GL_LEQUAL);
			irradianceRenderShader([&] {
				send("layer", index);
				send("isDistance", false);
				glBindTextureUnit(0, irradianceProbeGrid.texture());
				send(scene->activeCamera());
				shade(cube);
			});
			glDepthFunc(GL_LESS);
		}

		void LightFieldProbes::renderMeanDistance(int index) {
			glDepthFunc(GL_LEQUAL);
			irradianceRenderShader([&] {
				send("layer", index);
				send("isDistance", true);
				glBindTextureUnit(0, meanDistanceProbeGrid.texture());
				send(scene->activeCamera());
				shade(cube);
				});
			glDepthFunc(GL_LESS);
		}

		void LightFieldProbes::draw(Shader& shader) {
			for (auto& probe : probeInfos) {
				auto model = glm::translate(glm::mat4{ 1 }, probe.location);
				shader.sendUniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(model));
				sphere.draw(shader);
			}
		}

		void LightFieldProbes::sendTo(Shader& shader) {
			glActiveTexture(TEXTURE(lightFieldSurface.radianceProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_2D_ARRAY, lightFieldSurface.radianceProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.radianceProbeGrid.sampler", lightFieldSurface.radianceProbeGrid.texture.unit);
			shader.sendUniform4fv("lightFieldSurface.radianceProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.radianceProbeGrid.size));
			shader.sendUniform4fv("lightFieldSurface.radianceProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.radianceProbeGrid.invSize));


			glActiveTexture(TEXTURE(lightFieldSurface.normalProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_2D_ARRAY, lightFieldSurface.normalProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.normalProbeGrid.sampler", lightFieldSurface.normalProbeGrid.texture.unit);
			shader.sendUniform3fv("lightFieldSurface.normalProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.normalProbeGrid.size));
			shader.sendUniform3fv("lightFieldSurface.normalProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.normalProbeGrid.invSize));
			shader.sendUniform4fv("lightFieldSurface.normalProbeGrid.readMultiplyFirst", 1, glm::value_ptr(lightFieldSurface.normalProbeGrid.readMultiplyFirst));
			shader.sendUniform4fv("lightFieldSurface.normalProbeGrid.readAddSecond", 1, glm::value_ptr(lightFieldSurface.normalProbeGrid.readAddSecond));

			glActiveTexture(TEXTURE(lightFieldSurface.distanceProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_2D_ARRAY, lightFieldSurface.distanceProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.distanceProbeGrid.sampler", lightFieldSurface.distanceProbeGrid.texture.unit);
			shader.sendUniform3fv("lightFieldSurface.distanceProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.distanceProbeGrid.size));
			shader.sendUniform3fv("lightFieldSurface.distanceProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.distanceProbeGrid.invSize));

			glActiveTexture(TEXTURE(lightFieldSurface.lowResolutionDistanceProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_2D_ARRAY, lightFieldSurface.lowResolutionDistanceProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.lowResolutionDistanceProbeGrid.sampler", lightFieldSurface.lowResolutionDistanceProbeGrid.texture.unit);
			shader.sendUniform3fv("lightFieldSurface.lowResolutionDistanceProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.lowResolutionDistanceProbeGrid.size));
			shader.sendUniform3fv("lightFieldSurface.lowResolutionDistanceProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.lowResolutionDistanceProbeGrid.invSize));

			glActiveTexture(TEXTURE(lightFieldSurface.irradianceProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, lightFieldSurface.irradianceProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.irradianceProbeGrid.sampler", lightFieldSurface.irradianceProbeGrid.texture.unit);
			shader.sendUniform2fv("lightFieldSurface.irradianceProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.irradianceProbeGrid.size));
			shader.sendUniform2fv("lightFieldSurface.irradianceProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.irradianceProbeGrid.invSize));

			glActiveTexture(TEXTURE(lightFieldSurface.meanDistProbeGrid.texture.unit));
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, lightFieldSurface.meanDistProbeGrid.texture.buffer);
			shader.sendUniform1i("lightFieldSurface.meanDistProbeGrid.sampler", lightFieldSurface.meanDistProbeGrid.texture.unit);
			shader.sendUniform2fv("lightFieldSurface.meanDistProbeGrid.size", 1, glm::value_ptr(lightFieldSurface.meanDistProbeGrid.size));
			shader.sendUniform2fv("lightFieldSurface.meanDistProbeGrid.invSize", 1, glm::value_ptr(lightFieldSurface.meanDistProbeGrid.invSize));

			shader.sendUniform3iv("lightFieldSurface.probeCounts", 1, glm::value_ptr(lightFieldSurface.probeCounts));
			shader.sendUniform3fv("lightFieldSurface.probeStartPosition", 1, glm::value_ptr(lightFieldSurface.probeStartPosition));
			shader.sendUniform3fv("lightFieldSurface.probeStep", 1, glm::value_ptr(lightFieldSurface.probeStep));
			shader.sendUniform1i("lightFieldSurface.lowResolutionDownsampleFactor", lightFieldSurface.lowResolutionDownsampleFactor);

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
			auto oConfig = FrameBuffer::Config{ config.octResolution, config.octResolution };
			oConfig.fboTarget = GL_FRAMEBUFFER;
			oConfig.depthAndStencil = false;
			oConfig.depthTest = false;
			oConfig.stencilTest = false;

			for (int i = 0; i < 3; i++) {
				auto attachment = FrameBuffer::Attachment{};
				attachment.magFilter = GL_NEAREST;
				attachment.minfilter = GL_NEAREST;
				attachment.wrap_t = attachment.wrap_s = attachment.wrap_r = GL_CLAMP_TO_EDGE;
				attachment.texTarget = GL_TEXTURE_2D_ARRAY;
				attachment.internalFmt = GL_R11F_G11F_B10F;
				attachment.fmt = GL_RGB;
				attachment.type = GL_FLOAT;
				attachment.attachment = GL_COLOR_ATTACHMENT0 + i;
				attachment.texLevel = 0;
				attachment.numLayers = probeInfos.size();
				oConfig.attachments.push_back(attachment);
			}

			oConfig.attachments[0].mipMap = true;
			//oConfig.attachments[0].texLevel = 6;
			oConfig.attachments[2].internalFmt = GL_RG16F;
			oConfig.attachments[2].fmt = GL_RG;

			return oConfig;
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
			attachment.fmt = GL_RGB;
			attachment.type = GL_FLOAT;
			attachment.attachment = GL_COLOR_ATTACHMENT0;
			attachment.texLevel = 0;
			attachment.numLayers = probeInfos.size();
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
			attachment.numLayers = probeInfos.size();
			fConfig.attachments.push_back(attachment);

			return fConfig;
		}
	}	
}