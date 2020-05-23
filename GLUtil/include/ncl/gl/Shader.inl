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
#include <filesystem>
#include "common.h"

namespace ncl {
	namespace gl {

		Shader* Shader::boundShader;
		Shader* Shader::previouslyBoundShader = 0;

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

		static ncl::Logger stat_logger;

		Shader::Shader()
		{
			logger = ncl::Logger::get("Shader");
			stat_logger = logger;
			clear();
		}

		Shader::Shader(Shader&& source) noexcept {
			transfer(source, *this);
		}

		Shader& Shader::operator=(Shader&& source) noexcept {
			transfer(source, *this);
			return *this;
		}

		inline void transfer(Shader& source, Shader& dest) {
			dest._program = source._program;
			dest._totalShaders = source._totalShaders;

			for (int i = 0; i < NO_OF_SHADERS; i++) {
				dest._shaders[i] = source._shaders[i];
			}
			dest._attributeList = std::move(source._attributeList);
			dest._uniformLocationList = std::move(source._uniformLocationList);
			dest._subroutineList = std::move(source._subroutineList);
			dest.logger = source.logger;
			dest.pendingOps = std::move(source.pendingOps);
			dest._storePreprocessedShaders = source._storePreprocessedShaders;
			source._program = 0;
			source.clear();
		}

		GLint Shader::findUniformLocation(const std::string& name) {
			auto itr = _uniformLocationList.find(name);
			GLint location = -1;
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

		std::string preprocess(const std::string& source, const std::string& filename, std::set<std::string>& loaded, int level = 0);

		void Shader::load(const ShaderSource& source) {
			loadFromstring(source.ShaderType, source.data, source.filename);
		}

		void Shader::loadFromstring(GLenum type, const std::string& source, const std::string& filename) {
			GLuint shader = glCreateShader(type);

			std::set<std::string> loaded;
			const std::string newSource = preprocess(source, filename, loaded, 0);

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
			glDeleteShader(_shaders[TESELLATION_EVAL_SHADER]);
			glDeleteShader(_shaders[TESELLATION_CONTROL_SHADER]);
			glDeleteShader(_shaders[FRAGMENT_SHADER]);
			glDeleteShader(_shaders[GEOMETRY_SHADER]);
			glDeleteShader(_shaders[COMPUTE_SHADER]);

		}

		void Shader::relink() {
			glLinkProgram(_program);
		}

		void Shader::use(Procedure proc) {
			use();
			proc(*this);
			unUse();  
		}

		void Shader::use(Procedure1 proc) {
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
				previouslyBoundShader = boundShader;
				boundShader = this;
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
			boundShader = previouslyBoundShader;
			previouslyBoundShader = 0;
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

		GLint Shader::operator()(const std::string& uniform) {
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
			ncl::Logger::get("Shader").info("loading shader: " + filename);
			return ShaderSource{ shaderType,  ncl::getText(filename), filename };
		}

		void Shader::sendUniform1f(const std::string& name, GLfloat v0) {
			GLint location = findUniformLocation(name);
			glUniform1f(location, v0);
		}

		void Shader::sendUniform2f(const std::string& name, GLfloat v0, GLfloat v1) {
			GLint location = findUniformLocation(name);
			glUniform2f(location, v0, v1);
		}

		void Shader::sendUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2) {
			GLint location = findUniformLocation(name);
			glUniform3f(location, v0, v1, v2);
		}

		void Shader::sendUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
			GLint location = findUniformLocation(name);
			glUniform4f(location, v0, v1, v2, v3);
		}

		void Shader::sendUniform1fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniform1fv(location, count, value);
		}

		void Shader::sendUniform2fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniform2fv(location, count, value);
		}

		void Shader::sendUniform3fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniform3fv(location, count, value);
		}

		void Shader::sendUniform4fv(const std::string& name, GLsizei count, GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniform4fv(location, count, value);
		}

		void Shader::sendUniform1i(const std::string& name, GLint v0) {
			GLint location = findUniformLocation(name);
			glUniform1i(location, v0);
		}

		void Shader::sendUniform2i(const std::string& name, GLint v0, GLint v1) {
			GLint location = findUniformLocation(name);
			glUniform2i(location, v0, v1);
		}

		void Shader::sendUniform3i(const std::string& name, GLint v0, GLint v1, GLint v2) {
			GLint location = findUniformLocation(name);
			glUniform3i(location, v0, v1, v2);
		}

		void Shader::sendUniform4i(const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3) {
			GLint location = findUniformLocation(name);
			glUniform4i(location, v0, v1, v2, v3);
		}

		void Shader::sendUniform3iv(const std::string& name, GLsizei count, GLint* v) {
			GLint location = findUniformLocation(name);
			glUniform3iv(location, count, v);
		}

		void Shader::sendUniform1ui(const std::string& name, GLuint v0) {
			GLint location = findUniformLocation(name); // CHECK_GL_ERRORS
			glUniform1ui(location, v0); //CHECK_GL_ERRORS
		}

		void Shader::sendUniform2ui(const std::string& name, GLuint v0, GLuint v1) {
			GLint location = findUniformLocation(name);
			glUniform2ui(location, v0, v1);
		}

		void Shader::sendUniform3ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2) {
			GLint location = findUniformLocation(name);
			glUniform3ui(location, v0, v1, v2);
		}

		void Shader::sendUniform4ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
			GLint location = findUniformLocation(name);
			glUniform4ui(location, v0, v1, v2, v3);
		}

		void Shader::sendUniformMatrix2fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniformMatrix2fv(location, count, transpose, value);
		}

		void Shader::sendUniformMatrix3fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniformMatrix3fv(location, count, transpose, value);
		}

		void Shader::sendUniformMatrix4fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value) {
			GLint location = findUniformLocation(name);
			glUniformMatrix4fv(location, count, transpose, value);
		}

		void Shader::sendBool(const std::string& name, bool value) {
			GLint location = findUniformLocation(name);
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
			sendUniform1i(name + ".transform", light.transform);
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
			sendBool(name + ".ambientMap", material.ambientMat != -1);
			sendBool(name + ".diffuseMap", material.diffuseMat != -1);
			sendBool(name + ".specularMap", material.specularMat != -1);
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


		void Shader::send(const Camera& camera, const glm::mat4& model) {
			using namespace glm;
			sendUniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(model));
			sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));
			sendUniformMatrix4fv("V", 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		}

		void Shader::sendComputed(const Camera& camera, const glm::mat4& model) {
			using namespace glm;

			auto V = camera.getViewMatrix();
			auto P = camera.getProjectionMatrix();
			auto MV = V * model;
			auto MVP = P * MV;
			auto NM = inverseTranspose(mat3(MV));

			send(camera, model);
			sendUniformMatrix4fv("MV", 1, GL_FALSE, glm::value_ptr(MV));
			sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(MVP));
			sendUniformMatrix3fv("normalMatrix", 1, GL_FALSE, glm::value_ptr(NM));
		}

		void Shader::send(const GlmCam& camera) {
			sendUniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(camera.model));
			sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(camera.projection));
			sendUniformMatrix4fv("V", 1, GL_FALSE, glm::value_ptr(camera.view));
		}

		void Shader::sendComputed(const GlmCam& camera) {
			sendUniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(camera.model));
			sendUniformMatrix4fv("P", 1, GL_FALSE, glm::value_ptr(camera.projection));
			sendUniformMatrix4fv("V", 1, GL_FALSE, glm::value_ptr(camera.view));
			sendUniformMatrix4fv("MV", 1, GL_FALSE, glm::value_ptr(camera.view * camera.model));
			sendUniformMatrix4fv("MVP", 1, GL_FALSE, glm::value_ptr(camera.projection * camera.view * camera.model));
			sendUniformMatrix3fv("normalMatrix", 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(glm::mat3(camera.view * camera.model))));
		}

		void Shader::send(const LightModel& lightModel) {
			sendBool("lightModel.localViewer", lightModel.localViewer);
			sendBool("lightModel.twoSided", lightModel.twoSided);
			sendBool("lightModel.useObjectSpace", lightModel.useObjectSpace);
			sendBool("lightModel.celShading", lightModel.celShading);
			sendBool("lightModel.colorMaterial", lightModel.colorMaterial);
			sendUniform4fv("lightModel.globalAmbience", 1, (float*)&lightModel.globalAmbience[0]);
		}

		inline std::string processForEach(std::stringstream& in, std::string genTypeDef) {
			using namespace std;

			const regex FOR_EACH_END("^#endforeach");
			smatch for_each_end_match;
			stringstream template_out;
			string line = "";
			while (!regex_search(line, for_each_end_match, FOR_EACH_END)) {
				getline(in, line);
				if (line.find("#endforeach") == string::npos) {
					template_out << line << "\n";
				}
			}

			auto _template = template_out.str();

			const regex GEN_TYPE("\([a-zA-Z0-9]*\)");
			smatch match;
			string open = "(";
			string close = ")";
			stringstream out;
			string placeholder = "$(gentype)";

			if (regex_search(genTypeDef, match, GEN_TYPE)) {
				auto pos = 0;
				while (true) {
					pos = genTypeDef.find(open, pos);
					if (pos == string::npos) break;
					auto startIndex = pos + 1;
					auto endIndex = genTypeDef.find(close, pos);
					auto genType = genTypeDef.substr(startIndex, endIndex - startIndex);
					pos = endIndex;
					if (genType == "gentype") continue;
					
					auto templatePos = 0;
					auto source = _template;
					while (true) {
						templatePos = source.find(placeholder, templatePos);
						if (templatePos == string::npos) break;
						source.replace(templatePos, placeholder.length(), genType);
					}
					out << source;
					out << endl;
				}
			}

			return out.str();
		}

		inline std::string preprocess(const std::string& source, const std::string& filename, std::set<std::string>& loaded, int level) {
			static unsigned num = 0;
			if (level > 32) {
				throw "header inclusion depth limit reached, might be caused by cyclic header inclusion";
			}

			using namespace std;
			static const regex INCLUDE_PATTERN("^#pragma\\s*include\\s*\\(\\s*\"([A-Za-z0-9_.-]+\\.[A-za-z]+)\"\\.*\\)\\.*");
			const regex DEBUG_MODE_REGEX("^#pragma\\s*storeIntermediate\\(on\\)");
			const regex IGNORE_BEGIN("^#pragma\\s*ignore\\(on\\)");
			const regex IGNORE_END("^#pragma\\s*ignore\\(off\\)");
			const regex FOR_EACH_BEGIN("^#foreach.*");
			
			stringstream in;
			stringstream out;
			in << source;

			size_t line_number = 1;
			smatch matches;
			smatch debug_match;
			smatch ignore_match_on;
			smatch ignore_match_off;
			smatch for_each_match;

			auto find = [](string name) -> string {
				std::vector<std::filesystem::path> shader_loc = {
					"shaders\\" + name,
					"..\\shaders\\" + name,
					"C:\\Users\\" + username + "\\OneDrive\\cpp\\include\\shaders\\" + name
				};
				for (auto path : shader_loc) {
					if (std::filesystem::exists(path)) return path.string();
				}
				throw std::runtime_error(("unable to find shader: " + name).c_str());
				 
			};

			bool storeIntermidate = false;
			bool ignore = false;

			string line;
			while (getline(in, line)) {
				if (regex_search(line, debug_match, DEBUG_MODE_REGEX) && level == 0) {
					storeIntermidate = true;
				}
				if (regex_search(line, ignore_match_on, IGNORE_BEGIN)) {
					ignore = true;
				}

				if (regex_search(line, matches, INCLUDE_PATTERN)) {
					string file = matches[1];
					string include_file = find(file);
					auto itr = loaded.find(include_file);
					if (itr != loaded.end()) continue;	
					loaded.insert(include_file);
					string include_string = ncl::getText(include_file);
					out << preprocess(include_string, include_file, loaded, level + 1) << endl;
				}
				else if(!ignore && line.find("#foreach") == string::npos) {
					out << line << endl;
				}

				if (regex_search(line, for_each_match, FOR_EACH_BEGIN)) {
					out << processForEach(in, line);
				}

				if (regex_search(line, ignore_match_off, IGNORE_END)) {
					ignore = false;
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
				storeIntermidate = false;
			}
			return result;
		}
	}
}
