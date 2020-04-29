#pragma once

#include <string>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "textures.h"
#include "Scene.h"
#include "Shader.h"
#include "StorageBufferObj.h"
#include "FrameBuffer.h"
#include "shaders.h"
#include "mesh.h"

namespace ncl {
	namespace pbr {

		constexpr int ALBEDO_ID = 0;
		constexpr int NORMAL_ID = 1;
		constexpr int METALNESS_ID = 2;
		constexpr int ROUGHNESS_ID = 3;
		constexpr int AMBIENT_OCCLU_ID = 4;
		constexpr int MAX_SCENE_LIGHTS = 100;

		using Padding_f = float;

#pragma pack(show)
#pragma pack(push, 1)
		struct Light {
			glm::vec3 position;
			Padding_f padding0;
			glm::vec3 Intensity;
			Padding_f padding1;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Scene {
			glm::vec3 eyes;
			Padding_f padding0;
			Light lights[MAX_SCENE_LIGHTS];
		};
#pragma pack(pop)

		class Material {
		public:
			using Albedo = std::variant<std::string, glm::vec3>;
			using Metalness = std::variant<std::string, float>;
			using Roughness = std::variant<std::string, float>;
			using Normal = std::variant<std::string, glm::vec3>;
			using AmbientOcculusion = std::variant<std::string, float>;
			virtual void sendTo(gl::Shader& shader) = 0;
			virtual std::string name() = 0;
		};

		class ScalarMaterial : public Material {
		public:
			ScalarMaterial() = default;

			ScalarMaterial(std::string name, glm::vec3 albedo, float metalness, float roughness);

			void sendTo(gl::Shader& shader) override;

			std::string name() override {
				return _name;
			}

			glm::vec3 albedo;
			float metalness;
			float roughness;
			glm::vec3 ambientOcclu;
			std::string _name;
		};

		class TextureMaterial : public Material {
		public:
			TextureMaterial() = default;

			TextureMaterial(std::string name, glm::vec3 albedo, float metalness, float roughness, int width, int height);

			TextureMaterial(std::string name, std::string albedoPath, std::string normalPath, std::string metalPath, std::string roughPath, std::string aoPath, bool invertBlack = false, bool glossiness = false);

			TextureMaterial(std::string name, Albedo albedo, Normal normal, Metalness metalness, Roughness roughness, AmbientOcculusion ao, bool invertBlack = false, bool glossiness = false);

			TextureMaterial(TextureMaterial&&) noexcept;

			~TextureMaterial();

			TextureMaterial(const TextureMaterial&) = delete;

			TextureMaterial& operator=(TextureMaterial&&) noexcept;

			TextureMaterial& operator=(const TextureMaterial&) = delete;

			void sendTo(gl::Shader& shader) override;

			friend void transfer(TextureMaterial&, TextureMaterial&);

			static GLubyte* fill(glm::vec3 value, int width, int height);

			inline std::string name() override {
				return _name;
			}

		private:
			std::string _name;
			gl::Texture2D* albedo;
			gl::Texture2D* normal;
			gl::Texture2D* metalness;
			gl::Texture2D* roughness;
			gl::Texture2D* ambientOcclusion;
			bool _invertBlack = false;
			bool _glossiness = false;
		};

		struct Visitor {
			gl::Texture2D* operator()(std::string);
			gl::Texture2D* operator()(float);
			gl::Texture2D* operator()(glm::vec3);

			std::string name;
			GLint id, iFormat;
			GLint width = 256;
			GLint height = 256;
		};

		static gl::Texture2D* generate_brdf_lookup_table(GLuint textureUnit);

		ScalarMaterial::ScalarMaterial(std::string name, glm::vec3 albedo, float metalness, float roughness)
			: _name{ name }
			, albedo{ albedo }
			, metalness{ metalness }
			, roughness{ roughness }
			, ambientOcclu{ glm::vec3(1) }{

		}

		void ScalarMaterial::sendTo(gl::Shader& shader) {
			shader.sendUniform3fv("material.albedo", 1, glm::value_ptr(albedo));
			shader.sendUniform1f("material.metalness", metalness);
			shader.sendUniform1f("material.roughness", roughness);
			shader.sendUniform3fv("material.ao", 1, glm::value_ptr(ambientOcclu));
			shader.sendUniform1i("glossiness", false);
			shader.sendUniform1i("invertBlack", false);
			shader.sendUniform3fv("ao", 1, glm::value_ptr(ambientOcclu));
		}

		TextureMaterial::TextureMaterial(std::string name, glm::vec3 albedo, float metalness, float roughness, int width, int height)
			: _name{ name } {
			void* data = fill(albedo, width, height);
			this->albedo = new gl::Texture2D(data, width, height, "albedoMap", ALBEDO_ID, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, glm::vec2{ GL_CLAMP_TO_EDGE });
		//	this->albedo = new gl::CheckerTexture(ALBEDO_ID, "albedoMap");

			data = fill(glm::vec3(metalness), width, height);
			this->metalness = new gl::Texture2D(data, width, height, "metalnessMap", METALNESS_ID, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE,glm::vec2{ GL_CLAMP_TO_EDGE });

			data = fill(glm::vec3(roughness), width, height);
			this->roughness = new gl::Texture2D(data, width, height, "roughnessMap", ROUGHNESS_ID, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE,glm::vec2{ GL_CLAMP_TO_EDGE });

			data = fill(glm::vec3(glm::vec3(0, 0, 1)), width, height);
			this->ambientOcclusion = new gl::Texture2D(data, width, height, "aoMap", AMBIENT_OCCLU_ID, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE,glm::vec2{ GL_CLAMP_TO_EDGE });
			this->normal = new gl::Texture2D(data, width, height, "normalMap", NORMAL_ID, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE,glm::vec2{ GL_CLAMP_TO_EDGE });
		}

		TextureMaterial::TextureMaterial(std::string name, std::string albedoPath, std::string normalPath, std::string metalPath, std::string roughPath, std::string aoPath, bool invertBlack, bool glossiness)
			: _name{ name } {
			this->albedo = new gl::Texture2D(albedoPath, ALBEDO_ID, "albedoMap", GL_SRGB8, GL_RGB);
			this->normal = new gl::Texture2D(normalPath, NORMAL_ID, "normalMap");
			this->metalness = new gl::Texture2D(metalPath, METALNESS_ID, "metalnessMap");
			this->roughness = new gl::Texture2D(roughPath, ROUGHNESS_ID, "roughnessMap");
			this->ambientOcclusion = new gl::Texture2D(aoPath, AMBIENT_OCCLU_ID, "aoMap");
			this->_invertBlack = invertBlack;
			this->_glossiness = glossiness;
		}

		TextureMaterial::TextureMaterial(std::string name, Albedo albedo, Normal normal, Metalness metalness, Roughness roughness, AmbientOcculusion ao, bool invertBlack, bool glossiness):
			_name{ name } {
			// TODO albedo has to be a path if any other is a value
			this->albedo = std::visit(Visitor{ "albedoMap", ALBEDO_ID, GL_SRGB8 }, albedo);
			int w = this->albedo->width();
			int h = this->albedo->height();
			this->normal = std::visit(Visitor{"normalMap", NORMAL_ID, GL_RGB, w, h }, normal);
			this->metalness = std::visit(Visitor{ "metalnessMap", METALNESS_ID, GL_RGB, w, h }, metalness);
			this->roughness = std::visit(Visitor{ "roughnessMap", ROUGHNESS_ID, GL_RGB, w, h }, roughness);
			this->ambientOcclusion = std::visit(Visitor{ "aoMap", AMBIENT_OCCLU_ID, GL_RGB, w, h }, ao);
			this->_invertBlack = invertBlack;
			this->_glossiness = glossiness;
		}

		TextureMaterial::TextureMaterial(TextureMaterial&& source) noexcept {
			transfer(source, *this);
		}

		TextureMaterial::~TextureMaterial() {
			delete albedo;
			delete normal;
			delete metalness;
			delete roughness;
			delete ambientOcclusion;
		}

		TextureMaterial& TextureMaterial::operator=(TextureMaterial&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		GLubyte* TextureMaterial::fill(glm::vec3 value, int width, int height) {
			GLubyte* data = new GLubyte[width * height * 3];
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					int index = (i * width + j) * 3;
					data[index] = value.r * 255;
					data[index + 1] = value.g * 255;
					data[index + 2] = value.b * 255;
				}
			}
			return data;
		}

		void TextureMaterial::sendTo(gl::Shader& shader) {
			albedo->sendTo(shader);
			normal->sendTo(shader);
			metalness->sendTo(shader);
			roughness->sendTo(shader);
			ambientOcclusion->sendTo(shader);
			shader.sendUniform1i("glossiness", _glossiness);
			shader.sendUniform1i("invertBlack", _invertBlack);
		}

		inline void transfer(TextureMaterial& source, TextureMaterial& dest) {
			dest._name = std::move(source._name);
			dest.albedo = source.albedo;
			dest.normal = source.normal;
			dest.metalness = source.metalness;
			dest.roughness = source.roughness;
			dest.ambientOcclusion = source.ambientOcclusion;
			dest._glossiness = source._glossiness;
			dest._invertBlack = source._invertBlack;

			source.albedo = nullptr;
			source.normal = nullptr;
			source.metalness = nullptr;
			source.roughness = nullptr;
			source.ambientOcclusion = nullptr;
		}

		gl::Texture2D* Visitor::operator()(std::string path) {
			return  new gl::Texture2D(path, id, name, iFormat);
		}

		gl::Texture2D* Visitor::operator()(glm::vec3 v) {
			void* data = TextureMaterial::fill(v, width, height);
			return new gl::Texture2D(data, width, height, name, id, iFormat, GL_RGB, GL_UNSIGNED_BYTE, glm::vec2{ GL_CLAMP_TO_EDGE });
		}

		gl::Texture2D* Visitor::operator()(float v) {
			void* data = TextureMaterial::fill(glm::vec3(v), width, height);
			return new gl::Texture2D(data, width, height, name, id, iFormat, GL_RGB, GL_UNSIGNED_BYTE, glm::vec2{ GL_CLAMP_TO_EDGE });
		}

		inline static gl::Texture2D* generate_brdf_lookup_table(GLuint textureUnit) {
			gl::Mesh mesh;

			mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
			mesh.positions.emplace_back(-1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(-1.0f, 1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, -1.0f, 0.0f);
			mesh.positions.emplace_back(1.0f, 1.0f, 0.0f);

			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(0.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(0.0f, 1.0f);
			mesh.uvs[0].emplace_back(1.0f, 0.0f);
			mesh.uvs[0].emplace_back(1.0f, 1.0f);

			gl::ProvidedMesh* quad = new gl::ProvidedMesh{ mesh };

			gl::Shader shader;
			shader.load(gl::ShaderSource{ GL_VERTEX_SHADER, brdf_vert_shader, "brdf.vert" });
			shader.load(gl::ShaderSource{ GL_FRAGMENT_SHADER, bsdf_frag_shader, "brdf.frag" });
			shader.createAndLinkProgram();

			
			gl::FrameBuffer::Config config{ 512, 512 };
			config.clearColor = glm::vec4(1);
			config.deleteTexture = false;
			gl::FrameBuffer fbo = gl::FrameBuffer{ config };

			fbo.use([&]() {
				shader([&]() {
					shader.sendComputed(gl::GlmCam{});
					quad->draw(shader);
				});
			});



			
			return new gl::Texture2D{fbo.texture(), textureUnit};
		}
	}
}

template<>
struct ncl::gl::ObjectReflect<ncl::pbr::Scene> {

	static GLsizeiptr sizeOfObj(ncl::pbr::Scene& obj) {
		return sizeof(glm::vec4) + sizeof(ncl::pbr::Light) * ncl::pbr::MAX_SCENE_LIGHTS;
	}

	static void* objPtr(ncl::pbr::Scene& obj) {
		return &obj;
	}
};