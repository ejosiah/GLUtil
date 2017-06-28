#pragma once

#include <iterator>
#include <stdexcept>
#include <string>
#include <set>
#include "logger.h"
#include "util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <sstream>
#include <regex>
#include <boost/filesystem.hpp>

namespace ncl {
	namespace gl {

		std::stack<GLuint> Shader::activePrograms;

		std::string shaderName(GLenum shaderType) {
			switch (shaderType) {
			case GL_VERTEX_SHADER: return "Vertex shader";
			case GL_FRAGMENT_SHADER: return "Fragment shader";
			case GL_GEOMETRY_SHADER: return "Geometry shader";
			case GL_COMPUTE_SHADER: return "Compute shader";
			case GL_TESS_CONTROL_SHADER: return "Tessillation controll shader";
			case GL_TESS_EVALUATION_SHADER: return "Tessillation evaluation shader";
			}
		}

		Shader::Shader()
		{
			logger = ncl::Logger::get("Shader");
			clear();
		}

		GLuint Shader::findUniformLocation(const std::string& name) {
			auto itr = _uniformLocationList.find(name);
			GLuint location = -1;
			if (itr == _uniformLocationList.end()) {
				addUniform(name);
			}
			return _uniformLocationList[name];
		}

		GLuint Shader::findSubroutineLocation(const std::string& name, GLenum shaderType) {
			auto itr = _subroutineList[shaderType].find(name);
			if (itr != _subroutineList[shaderType].end()) {
				return glGetSubroutineUniformLocation(_program, shaderType, itr->second.c_str());
			}
			auto error = std::runtime_error("No subroute defined for function: " + name);
			logger.error("", error);
		}

		Shader::~Shader(void)
		{
			clear();
		}

		void Shader::clear() {
			_attributeList.clear();
			_uniformLocationList.clear();
			
			_program = 0;
			_totalShaders = 0;
			_shaders[VERTEX_SHADER] = 0;
			_shaders[FRAGMENT_SHADER] = 0;
			_shaders[TESELLATION_CONTROL_SHADER] = 0;
			_shaders[TESELLATION_EVAL_SHADER] = 0;
			_shaders[GEOMETRY_SHADER] = 0;
			_shaders[COMPUTE_SHADER] = 0;
			_attributeList.clear();
			_uniformLocationList.clear();
			if (_program != 0) {
				glDeleteProgram(_program);
				_program = 0;
			}
			if (activePrograms.empty()) activePrograms.push(0);
		}

		std::string preprocess(const std::string& source, const std::string& filename, std::set<std::string>& loaded, int level = 0, bool storeIntermidate = false);

		void Shader::load(const ShaderSource& source) {
			loadFromstring(source.ShaderType, source.data, source.filename);
		}

		void Shader::loadFromstring(GLenum type, const std::string& source, const std::string& filename) {
			GLuint shader = glCreateShader(type);

			std::set<std::string> loaded;
			const std::string newSource = preprocess(source, filename, loaded, 0, _storePreprocessedShaders);

			const char * ptmp = newSource.c_str();
			glShaderSource(shader, 1, &ptmp, NULL);

			//check whether the shader loads fine
			GLint status;
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				GLint infoLogLength;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
				GLchar *infoLog = new GLchar[infoLogLength];
				glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
				std::string msg = shaderName(type) + ": " + filename +  " Compile log:\n";
				msg.append(infoLog);
				logger.error(msg);
				delete[] infoLog;
				throw std::runtime_error(msg);

			}
			_shaders[_totalShaders++] = shader;
			_subroutineList[type] = std::map<std::string, std::string>();
		}

		void Shader::createAndLinkProgram() {
			_program = glCreateProgram();
			if (_shaders[VERTEX_SHADER] != 0) {
				glAttachShader(_program, _shaders[VERTEX_SHADER]);
			}
			if (_shaders[FRAGMENT_SHADER] != 0) {
				glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
			}
			if (_shaders[TESELLATION_CONTROL_SHADER] != 0) {
				glAttachShader(_program, _shaders[TESELLATION_CONTROL_SHADER]);
			}
			if (_shaders[TESELLATION_EVAL_SHADER] != 0) {
				glAttachShader(_program, _shaders[TESELLATION_EVAL_SHADER]);
			}
			if (_shaders[GEOMETRY_SHADER] != 0) {
				glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
			}
			if (_shaders[COMPUTE_SHADER] != 0) {
				glAttachShader(_program, _shaders[COMPUTE_SHADER]);
			}

			//link and check whether the program links fine
			GLint status;
			glLinkProgram(_program);
			glGetProgramiv(_program, GL_LINK_STATUS, &status);
			if (status == GL_FALSE) {
				GLint infoLogLength;

				glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
				GLchar *infoLog = new GLchar[infoLogLength];
				glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
				std::string msg = "Link log: ";
				msg.append(infoLog);
				delete[] infoLog;
				logger.error(msg);
				throw std::runtime_error(msg);
				delete[] infoLog;
			}

			glDeleteShader(_shaders[VERTEX_SHADER]);
			glDeleteShader(_shaders[FRAGMENT_SHADER]);
			glDeleteShader(_shaders[GEOMETRY_SHADER]);
			glDeleteShader(_shaders[COMPUTE_SHADER]);
		}

		void Shader::use(Procedure proc) {
			use();
			proc();
			unUse();
            
		}

		void Shader::use() {
			if (!isActive()) return;
			if (_program == 0) {
				throw std::runtime_error("Shader not yet initialized");
			}
			if (activePrograms.top() != _program) {
				activePrograms.push(_program);
				glUseProgram(_program);
			}
		}


		void Shader::unUse() {
			if (!isActive()) return;
			if (_program == 0) {
				throw std::runtime_error("Shader not yet initialized");
			}
			activePrograms.pop();
			GLuint program = activePrograms.top();
			
	/*		if (activePrograms.size() > 1) {
				program = activePrograms.top();
				activePrograms.pop();
			}
			else {
				activePrograms.pop();
			}*/

			glUseProgram(program);
		}

		void Shader::addAttribute(const std::string& attribute, const GLuint location) {
			_attributeList[attribute] = location;
		}

		//An indexer that returns the location of the attribute
		GLuint Shader::operator [](const std::string& attribute) {
			return _attributeList[attribute];
		}

		void Shader::addUniform(const std::string& uniform) {
			_uniformLocationList[uniform] = glGetUniformLocation(_program, uniform.c_str());
		}

		void Shader::addSubroutineLocation(GLenum shaderType, const std::string& subroutine, std::initializer_list<std::string> functions) {
			for (std::string f : functions) {
				_subroutineList[shaderType][f] = subroutine;
			}
		}

		GLuint Shader::operator()(const std::string& uniform) {
			return _uniformLocationList[uniform];
		}

		void Shader::loadFromFiles(std::vector<std::string> filenames) {

			for (std::string filename : filenames) {
				loadFromFile(filename);
			}
		}

		bool Shader::loadFromFile(const std::string& filename) {
			if (isShader(filename)) {
				auto key = extractExt(filename);
				GLenum whichShader = extensions.at(key);
				logger.info("loading shader: " + filename);
				loadFromstring(whichShader, ncl::getText(filename), filename);
				return true;
			}
			return false;
		}

		bool Shader::isShader(const std::string filename) {
			auto key = extractExt(filename);
			auto itr = extensions.find(key);
			return itr != extensions.end();
		}

		ShaderSource Shader::extractFromFile(const std::string& filename) {
			auto key = extractExt(filename);
			GLenum shaderType = extensions.at(key);
			logger.info("loading shader: " + filename);
			return ShaderSource{ shaderType,  ncl::getText(filename), filename };
		}

		void Shader::sendUniform1f(const std::string& name, GLfloat v0) {
			GLuint location = findUniformLocation(name);
			glUniform1f(location, v0);
		}

		void Shader::sendUniform2f(const std::string& name, GLfloat v0, GLfloat v1) {
			GLuint location = findUniformLocation(name);
			glUniform2f(location, v0, v1);
		}

		void Shader::sendUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2) {
			GLuint location = findUniformLocation(name);
			glUniform3f(location, v0, v1, v2);
		}

		void Shader::sendUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
			GLuint location = findUniformLocation(name);
			glUniform4f(location, v0, v1, v2, v3);
		}

		void Shader::sendUniform1fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniform1fv(location, count, value);
		}

		void Shader::sendUniform2fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniform2fv(location, count, value);
		}

		void Shader::sendUniform3fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniform3fv(location, count, value);
		}

		void Shader::sendUniform4fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniform4fv(location, count, value);
		}

		void Shader::sendUniform1i(const std::string& name, GLint v0) {
			GLuint location = findUniformLocation(name);
			glUniform1i(location, v0);
		}

		void Shader::sendUniform2i(const std::string& name, GLint v0, GLint v1) {
			GLuint location = findUniformLocation(name);
			glUniform2i(location, v0, v1);
		}

		void Shader::sendUniform3i(const std::string& name, GLint v0, GLint v1, GLint v2) {
			GLuint location = findUniformLocation(name);
			glUniform3i(location, v0, v1, v2);
		}

		void Shader::sendUniform4i(const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3) {
			GLuint location = findUniformLocation(name);
			glUniform4i(location, v0, v1, v2, v3);
		}

		void Shader::sendUniform1ui(const std::string& name, GLuint v0) {
			GLuint location = findUniformLocation(name);
			glUniform1ui(location, v0);
		}

		void Shader::sendUniform2ui(const std::string& name, GLuint v0, GLuint v1) {
			GLuint location = findUniformLocation(name);
			glUniform2ui(location, v0, v1);
		}

		void Shader::sendUniform3ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2) {
			GLuint location = findUniformLocation(name);
			glUniform3ui(location, v0, v1, v2);
		}

		void Shader::sendUniform4ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
			GLuint location = findUniformLocation(name);
			glUniform4ui(location, v0, v1, v2, v3);
		}

		void Shader::sendUniformMatrix2fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniformMatrix2fv(location, count, transpose, value);
		}

		void Shader::sendUniformMatrix3fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniformMatrix3fv(location, count, transpose, value);
		}

		void Shader::sendUniformMatrix4fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLuint location = findUniformLocation(name);
			glUniformMatrix4fv(location, count, transpose, value);
		}

		void Shader::send(const std::string& name, bool value) {
			GLuint location = findUniformLocation(name);
			glUniform1i(location, value);
		}

		
		void Shader::sendUniformLight(const std::string& name, LightSource& light) {
			sendUniform4fv(name + ".position", 1, &light.position[0]);
			sendUniform4fv(name + ".ambient", 1, &light.ambient[0]);
			sendUniform4fv(name + ".diffuse", 1, &light.diffuse[0]);
			sendUniform4fv(name + ".specular", 1, &light.specular[0]);
			sendUniform4fv(name + ".spotDirection", 1, &light.spotDirection[0]);
			sendUniform1f(name + ".spotAngle", light.spotAngle);
			sendUniform1f(name + ".spotExponent", light.spotExponent);
			sendUniform1ui(name + ".transform", light.transform);
			sendUniform1i(name + ".on", light.on);
			sendUniform1f(name + ".kc", light.kc);
			sendUniform1f(name + ".ki", light.ki);
			sendUniform1f(name + ".kq", light.kq);
		}

		void Shader::sendUniformLights(LightSource lights[], int size) {
			for (int i = 0; i < size; i++) {
				sendUniformLight("light[" + std::to_string(i) + "]", lights[i]);
			}
		}

		void Shader::sendUniformMaterial(const std::string& name, Material& material) {
			sendUniform4fv(name + ".ambient", 1, &material.ambient[0]);
			sendUniform4fv(name + ".diffuse", 1, &material.diffuse[0]);
			sendUniform4fv(name + ".specular", 1, &material.specular[0]);
			sendUniform4fv(name + ".emission", 1, &material.emission[0]);
			sendUniform1f(name + ".shininess", material.shininess);
		}

		void Shader::sendUniformMaterials(const std::string& name, Material materials[]) {
			sendUniformMaterial("material[0]", materials[0]);
			sendUniformMaterial("material[1]", materials[1]);
		}

		void Shader::subroutine(const std::string& name, GLenum shaderType) {
			GLsizei n;
			glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &n);
			GLuint* indices = new GLuint[n];

			GLuint subroutinePtr = findSubroutineLocation(name, shaderType);
			
			GLuint funcLoc = glGetSubroutineIndex(_program, shaderType, name.c_str());
			indices[subroutinePtr] = funcLoc;
			glUniformSubroutinesuiv(shaderType, n, indices);
			delete[] indices;
		}

		void Shader::send(const Camera& camera, const Matrix4& model) {
			Matrix4 MV = model * camera.getViewMatrix();
			Matrix4 MVP = MV * camera.getProjectionMatrix();
			Matrix3 NM = Matrix3(MV).inverse().transpose();

			sendUniformMatrix4fv("V", 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
			sendUniformMatrix4fv("MV", 1, GL_FALSE, MV);
			sendUniformMatrix4fv("MVP", 1, GL_FALSE, MVP);
			sendUniformMatrix3fv("normalMatrix", 1, GL_FALSE, NM);

		}

		void Shader::send(const Camera2& camera, const glm::mat4& model) {
			using namespace glm;
			mat4 MV =  camera.getViewMatrix() * model;
			mat4 MVP =  camera.getProjectionMatrix() * MV;
			mat3 NM = inverseTranspose(mat3(camera.getViewMatrix() * model));

			sendUniformMatrix4fv("V", 1, GL_FALSE, value_ptr(camera.getViewMatrix()));
			sendUniformMatrix4fv("MV", 1, GL_FALSE, value_ptr(MV));
			sendUniformMatrix4fv("MVP", 1, GL_FALSE, value_ptr(MVP));
			sendUniformMatrix3fv("normalMatrix", 1, GL_FALSE, value_ptr(NM));
		}

		void Shader::send(const GlmCam& camera) {
			sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(camera.projection));
			sendUniformMatrix4fv("V", 1, GL_FALSE, glm::value_ptr(camera.view));
			sendUniformMatrix4fv("MV", 1, GL_FALSE, glm::value_ptr(camera.view * camera.model));
			sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(camera.projection * camera.view * camera.model));
			sendUniformMatrix3fv("normalMatrix", 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(camera.view * camera.model))));
		}

		void Shader::send(const LightModel& lightModel) {
			send("lightModel.localViewer", lightModel.localViewer);
			send("lightModel.twoSided", lightModel.twoSided);
			send("lightModel.useObjectSpace", lightModel.useObjectSpace);
			send("lightModel.celShading", lightModel.celShading);
			send("lightModel.colorMaterial", lightModel.colorMaterial);
			sendUniform4fv("lightModel.globalAmbience", 1, (float*)&lightModel.globalAmbience[0]);
		}
#ifndef SHADER_PROCESS
#define SHADER_PROCESS
		std::string preprocess(const std::string& source, const std::string& filename, std::set<std::string>& loaded, int level, bool storeIntermidate) {
			static unsigned num = 0;
			if (level > 32) {
				throw "header inclusion depth limit reached, might be caused by cyclic header inclusion";
			}

			using namespace std;
			static const regex INCLUDE_PATTERN("^#pragma\\s*include\\s*\\(\\s*\"([A-Za-z0-9_.-]+\\.[A-za-z]+)\"\\.*\\)\\.*");
			stringstream in;
			stringstream out;
			in << source;

			size_t line_number = 1;
			smatch matches;

			string line;
			while (getline(in, line)) {
				if (regex_search(line, matches, INCLUDE_PATTERN)) {
					string file = matches[1];
					string include_file = "C:\\Users\\Josiah\\OneDrive\\cpp\\include\\shaders\\" + file;
					auto itr = loaded.find(include_file);
					if (itr != loaded.end()) continue;	
					loaded.insert(include_file);
					string include_string = ncl::getText(include_file);
					out << preprocess(include_string, include_file, loaded, level + 1) << endl;
				}
				else {
					out << line << endl;
				}
				++line_number;
			}
			string result = out.str();
			if (storeIntermidate) {
				int i = filename.find_last_of("\\");
				int j = filename.find_last_of(".");
				int k = filename.length() - j + 1;
				string name = filename.substr(i + 1, k);
				string ext = filename.substr(j + 1, filename.length());
				ofstream fout;
				fout.open(name  +  ".intermediate." + ext);
				fout << result << endl;
				fout.flush();
				fout.close();
			}
			return result;
		}
#endif

	}
}
