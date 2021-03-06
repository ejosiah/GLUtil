#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <utility>
#include <set>
#include <stack>
#include <initializer_list>
#include <gl/gl_core_4_5.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "models.h"
#include <functional>
#include "Camera.h"
#include "logger.h"
#include "Camera.h"



namespace ncl {
	namespace gl {
		class Shader;
		static const unsigned NO_OF_SHADERS = 6;
        using Procedure = std::function<void(Shader& shader)>;
		using Procedure1 = std::function<void()>;
		const std::map<std::string, GLenum> extensions{
			std::make_pair("vert", GL_VERTEX_SHADER),
			std::make_pair("frag", GL_FRAGMENT_SHADER),
			std::make_pair("geom", GL_GEOMETRY_SHADER),
			std::make_pair("tcs", GL_TESS_CONTROL_SHADER),
			std::make_pair("tes", GL_TESS_EVALUATION_SHADER),
			std::make_pair("tesc", GL_TESS_CONTROL_SHADER),
			std::make_pair("tese", GL_TESS_EVALUATION_SHADER),
			std::make_pair("comp", GL_COMPUTE_SHADER)
		};

		struct GlmCam {
			glm::mat4 projection = glm::ortho(-1, 1, -1, 1);
			glm::mat4 view = glm::mat4(1);
			glm::mat4 model = glm::mat4(1);
			glm::mat4 viewport;
		};

		struct ShaderSource {
			GLenum ShaderType;
			std::string data;
			std::string filename;
			bool compile = true;
		};

		class Shader {
		protected:
			GLint findUniformLocation(const std::string& name);

		public:
			Shader();
			Shader(const ShaderSource& source);
			~Shader();

			Shader(Shader&& source) noexcept;
			Shader(const Shader&) = delete;

			Shader& operator=(Shader&& source) noexcept;

			Shader& operator=(const Shader&) = delete;

			friend void transfer(Shader& source, Shader& dest);

			void loadFromFiles(std::vector<std::string> filenames);

			void loadFromstring(GLenum shader, const std::string& source, const std::string& filename = ".shader");
			bool loadFromFile(const std::string& filename);
			void createAndLinkProgram();
			void relink();

			void initSubroutines();

			void use(Procedure proc);	// TODO delete this
			void use(Procedure1 proc);
			void use();
			void unUse();

			void addAttribute(const std::string& attribute, const GLuint location);
			void addUniform(const std::string& uniform);
			GLuint operator[] (const std::string& attribute);
			GLint operator() (const std::string& uniform);

			void sendUniform1f(const std::string& name, GLfloat v0);
			void sendUniform2f(const std::string& name, GLfloat v0, GLfloat v1);
			void sendUniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2);
			void sendUniform4f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

			void sendUniform1fv(const std::string& name, GLsizei count, GLfloat* value);
			void sendUniform2fv(const std::string& name, GLsizei count, GLfloat* value);
			void sendUniform3fv(const std::string& name, GLsizei count, GLfloat* value);
			void sendUniform4fv(const std::string& name, GLsizei count, GLfloat* value);

			void sendUniform1i(const std::string& name, GLint v0);
			void sendUniform2i(const std::string& name, GLint v0, GLint v1);
			void sendUniform3i(const std::string& name, GLint v0, GLint v1, GLint v2);
			void sendUniform4i(const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3);



			inline void sendUniform3iv(const std::string& name, GLint* v) {
				sendUniform3iv(name, 1, v);
			}

			void sendUniform3iv(const std::string& name, GLsizei count, GLint* v);

			void sendUniform1ui(const std::string& name, GLuint v0);
			void sendUniform2ui(const std::string& name, GLuint v0, GLuint v1);
			void sendUniform3ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2);
			void sendUniform4ui(const std::string& name, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

			void sendUniformMatrix2fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value);
			void sendUniformMatrix3fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value);
			void sendUniformMatrix4fv(const std::string& name, GLsizei count, GLboolean transpose, const GLfloat* value);
			void sendBool(const std::string& name, bool value);
			void subroutine(GLenum shaderType, const std::string& name, const std::string& func_name);


			void send(const LightModel& lightModel);

			inline void sendUniformLight(LightSource& light) { sendUniformLight("light[0]", light); }

			void sendUniformLight(const std::string& name, LightSource& light);

			void sendUniformLights(LightSource light[], int size);

			void sendUniformMaterial(const std::string& name, Material& material);
			void sendUniformMaterials(const std::string& name, Material material[]);


			void send(const Camera& camera, const glm::mat4& model = glm::mat4(1));

			void sendComputed(const Camera& camera, const glm::mat4& model = glm::mat4(1));

			void send(const GlmCam& camera);

			void sendComputed(const GlmCam& camera);

            inline void operator()(Procedure proc){
                use(proc);
            }

			inline void operator()(Procedure1 proc) {
				use(proc);
			}

			void clear();

			inline GLuint program() { return _program;  }

			inline bool isActive() { return _program != 0;  }

			inline static bool isShader(const std::string filename);

			inline void storePreprocessedShaders(bool flag) { _storePreprocessedShaders = flag;  }

			static ShaderSource extractFromFile(const std::string& filename);

			void load(const ShaderSource& source);

			static bool shouldCompile(const ShaderSource& source);

			static Shader* boundShader;
			static Shader* previouslyBoundShader;


		private:
			enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, TESELLATION_CONTROL_SHADER, TESELLATION_EVAL_SHADER, GEOMETRY_SHADER, COMPUTE_SHADER };
			GLuint _program;
			int _totalShaders;
			GLuint _shaders[NO_OF_SHADERS];
			std::map<std::string, GLuint> _attributeList;
			std::map<std::string, GLint> _uniformLocationList;
			std::map<GLenum, std::map<std::string, std::tuple<GLint, GLuint>>> _subroutinesPtrs;
			std::map<GLenum, std::vector<GLuint>> _subroutineIndexes;
			ncl::Logger logger;
			std::vector<Procedure> pendingOps;
			bool _storePreprocessedShaders = false;
			static std::stack<GLuint> activePrograms;
		};
	}
}

#include "Shader.inl"
// TODO bind active shader 
