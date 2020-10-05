#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <initializer_list>
#include "Shader.h"
#include "textures.h"
#include "BufferObject.h"
#include "pbr.h"
#include "ShadowMap.h"
//#include "light_field_probes.h"

namespace ncl {
	namespace gl {

		inline void ensureShaderbound() {
			if (Shader::boundShader == nullptr) {
				throw std::runtime_error("No shader active");
			}
		}

		inline void addAttribute(const std::string& attribute, const GLuint location) {
			ensureShaderbound();
			Shader::boundShader->addAttribute(attribute, location);
		}


		inline void addUniform(const std::string& uniform) {
			ensureShaderbound();
			Shader::boundShader->addUniform(uniform);
		}

	

		inline void send(const std::string& name, GLfloat v0) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1f(name, v0);
		}


		inline void send(const std::string& name, GLfloat v0, GLfloat v1) {
			ensureShaderbound();
			Shader::boundShader->sendUniform2f(name, v0, v1);
		}

		inline void sendUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2) {
			ensureShaderbound();
			Shader::boundShader->sendUniform3f(name, v0, v1, v2);
		}


		inline void sendUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
			ensureShaderbound();
			Shader::boundShader->sendUniform4f(name, v0, v1, v2, v3);
		}




		inline void send(const std::string& name, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1fv(name, 1, value);
		}

		inline void sendArray(const std::string& name, GLfloat* arr, GLsizei size) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1fv(name, size, arr);
		}
		
		inline void send(const std::string& name, GLsizei count, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1fv(name, count, value);
		}

		inline void send2fv(const std::string& name, GLsizei count, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform2fv(name, count, value);
		}


		inline void send2fv(const std::string& name, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform2fv(name, 1, value);
		}

		inline void send3fv(const std::string& name, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform3fv(name, 1, value);
		}

		inline void send3fv(const std::string& name, GLsizei count, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform3fv(name, count, value);
		}


		inline void send4fv(const std::string& name, GLsizei count, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform4fv(name, count, value);
		}

		inline void send4fv(const std::string& name, GLfloat* value) {
			ensureShaderbound();
			Shader::boundShader->sendUniform4fv(name, 1, value);
		}

		inline void send(const std::string& name, const glm::vec2& value) {
			send2fv(name, const_cast<float*>(glm::value_ptr(value)));
		}

		inline void send(const std::string& name, const glm::vec3& value) {
			send3fv(name,  const_cast<float*>(glm::value_ptr(value)));
		}

		inline void send(const std::string& name, const glm::vec4& value) {
			send4fv(name, const_cast<float*>(glm::value_ptr(value)));
		}

		inline void send(const std::string& name, GLint v0) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1i(name, v0);
		}


		inline void send(const std::string& name, GLint v0, GLint v1) {
			ensureShaderbound();
			Shader::boundShader->sendUniform2i(name, v0, v1);
		}

		inline void send(const std::string& name, GLint v0, GLint v1, GLint v2) {
			ensureShaderbound();
			Shader::boundShader->sendUniform3i(name, v0, v1, v2);
		}


		inline void send(const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3) {
			ensureShaderbound();
			Shader::boundShader->sendUniform4i(name, v0, v1, v2, v3);
		}

		inline void sendui(const std::string& name, GLint v0) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1ui(name, v0);
		}


		inline void sendui(const std::string& name, GLint v0, GLint v1) {
			ensureShaderbound();
			Shader::boundShader->sendUniform2ui(name, v0, v1);
		}

		inline void sendui(const std::string& name, GLint v0, GLint v1, GLint v2) {
			ensureShaderbound();
			Shader::boundShader->sendUniform3ui(name, v0, v1, v2);
		}


		inline void sendui(const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3) {
			ensureShaderbound();
			Shader::boundShader->sendUniform4ui(name, v0, v1, v2, v3);
		}

		inline void send(const std::string& name, size_t size) {
			ensureShaderbound();
			Shader::boundShader->sendUniform1i(name, int(size));
		}

		inline void send(const std::string& name, GLsizei count, GLboolean transpose, glm::mat2 value) {
			ensureShaderbound();
			Shader::boundShader->sendUniformMatrix2fv(name, count, transpose, glm::value_ptr(value));
		}

		inline void send(const std::string& name, GLboolean transpose, glm::mat2 value) {
			send(name, 1, transpose, value);
		}

		inline void send(const std::string& name, glm::mat2 value) {
			send(name, 1, false, value);
		}


		inline void send(const std::string& name, GLsizei count, GLboolean transpose, glm::mat3 value) {
			ensureShaderbound();
			Shader::boundShader->sendUniformMatrix3fv(name, count, transpose, glm::value_ptr(value));
		}

		inline void send(const std::string& name, GLboolean transpose, glm::mat3 value) {
			send(name, 1, transpose, value);
		}

		inline void send(const std::string& name, glm::mat3 value) {
			send(name, false, value);
		}

		inline void send(const std::string& name, GLsizei count, GLboolean transpose, glm::mat4 value) {
			ensureShaderbound();
			Shader::boundShader->sendUniformMatrix4fv(name, count, transpose, glm::value_ptr(value));
		}

		inline void send(const std::string& name, GLboolean transpose, glm::mat4 value) {
			send(name, 1, transpose, value);
		}

		inline void send(const std::string& name, glm::mat4 value) {
			send(name, false, value);
		}


		inline void send(const LightModel& lightModel) {
			ensureShaderbound();
			Shader::boundShader->send(lightModel);
		}

		inline void send(LightSource& light) { 
			ensureShaderbound();
			Shader::boundShader->sendUniformLight("light[0]", light); 
		}

		inline void send(const std::string& name, LightSource& light) {
			ensureShaderbound();
			Shader::boundShader->sendUniformLight(name, light);
		}

		inline void send(LightSource light[], int size) {
			ensureShaderbound();
			Shader::boundShader->sendUniformLights(light, size);
		}

		inline void send(const std::string& name, Material& material) {
			ensureShaderbound();
			Shader::boundShader->sendUniformMaterial(name, material);
		}


		inline void sendUniformMaterials(const std::string& name, Material material[]) {
			ensureShaderbound();
			Shader::boundShader->sendUniformMaterials(name, material);
		}

		inline void send(Camera* camera, const glm::mat4& model = glm::mat4(1)) {
			ensureShaderbound();
			Shader::boundShader->sendComputed(*camera, model);
		}

		inline void send(const Camera& camera, const glm::mat4& model = glm::mat4(1)){
			ensureShaderbound();
			Shader::boundShader->sendComputed(camera, model);
		}

		inline void send(GlmCam& cam) {
			ensureShaderbound();
			Shader::boundShader->sendComputed(cam);
		}

		inline void send(TextureBuffer* buffer) {
			buffer->sendTo(*Shader::boundShader);
		}

		inline void send(Texture2D* texutre) {
			ensureShaderbound();
			texutre->sendTo(*Shader::boundShader);
		}

		inline void send(Image2D* image) {
			ensureShaderbound();
			image->sendTo(*Shader::boundShader);
		}

		inline void shade(Drawable& drawable) {
			ensureShaderbound();
			drawable.draw(*Shader::boundShader);
		}

		//[[ deprecated ]]
		inline void shade(Drawable* drawable) {
			shade(*drawable);
		}
		
		inline void shade(std::initializer_list<Drawable*> drawables) {
			ensureShaderbound();
			for (auto* drawable : drawables) {
				shade(drawable);
			}
		}

		template<typename T>
		inline void  send(StorageBufferObj<T>& obj) {
			ensureShaderbound();
			obj.sendToGPU();
		}

		inline void send(pbr::Material& material) {
			ensureShaderbound();
			material.sendTo(*Shader::boundShader);
		}

		inline void send(ShadowMap& shadowMap) {
			ensureShaderbound();
			shadowMap.sendTo(*Shader::boundShader);
		}

		inline void send(ShadowMap* shadowMap) {
			ensureShaderbound();
			shadowMap->sendTo(*Shader::boundShader);
		}

		inline void clearBindings() {
			for (int i = 0; i < 16; i++) {
				glBindTextureUnit(i, 0);
			}
		}


		inline void subroutine(GLenum shaderType, const std::string& name, const std::string& func_name) {
			ensureShaderbound();
			Shader::boundShader->subroutine(shaderType, name, func_name);
		}

		inline void subroutineFrag(const std::string& name, const std::string& func_name) {
			subroutine(GL_FRAGMENT_SHADER, name, func_name);
		}

		inline void subroutineVert(const std::string& name, const std::string& func_name) {
			subroutine(GL_VERTEX_SHADER, name, func_name);
		}

		inline void subroutineGeom(const std::string& name, const std::string& func_name) {
			subroutine(GL_GEOMETRY_SHADER, name, func_name);
		}
		
		inline void subroutineTessEval(const std::string& name, const std::string& func_name) {
			subroutine(GL_TESS_EVALUATION_SHADER, name, func_name);
		}

		inline void subroutineTessCtl(const std::string& name, const std::string& func_name) {
			subroutine(GL_TESS_CONTROL_SHADER, name, func_name);
		}

		inline void subroutineCompute(const std::string& name, const std::string& func_name) {
			subroutine(GL_COMPUTE_SHADER, name, func_name);
		}

	}
}