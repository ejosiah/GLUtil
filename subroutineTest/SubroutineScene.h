#pragma once

#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

struct Subroutine {
	GLenum shaderType;
	GLint loc;
	GLint index;
	std::string name;
};

class SubRoutineScene : public Scene {
public:
	SubRoutineScene() :Scene("Subroutine Test") {
		addShader("subroutine", GL_VERTEX_SHADER, screen_vert_shader);
	}

	void init() override {
		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);
		GLuint program;
		//shader("subroutine")([&] {
		//	program = shader("subroutine").program();
		//	colorLoc = glGetSubroutineUniformLocation(program, GL_FRAGMENT_SHADER, "color");
		//	assert(colorLoc >= 0);
		//	
		//	redIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "red");
		//	assert(redIndex != GL_INVALID_INDEX);
		//
		//	greenIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "green");
		//	assert(greenIndex != GL_INVALID_INDEX);
		//
		//	blueIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "blue");
		//	assert(blueIndex != GL_INVALID_INDEX);

		//	

		//	subLoc = glGetSubroutineUniformLocation(program, GL_FRAGMENT_SHADER, "sub_color");
		//	assert(colorLoc >= 0);
		//	yellowIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "yellow");
		//	assert(yellowIndex != GL_INVALID_INDEX);
		//	cyanIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "cyan");
		//	assert(cyanIndex != GL_INVALID_INDEX);

		//	logSubroutine(program);
		//});

	}

	void logSubroutine(GLuint p) {
		int maxSub, maxSubU, activeS, countActiveSU;
		char name[256]; int len, numCompS;

		glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
		glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
		logger.info("Max Subroutines: " + to_string(maxSub) +  "  Max Subroutine Uniforms:" + to_string(maxSubU));

		glGetProgramStageiv(p, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);

		for (int i = 0; i < countActiveSU; ++i) {

			glGetActiveSubroutineUniformName(p, GL_FRAGMENT_SHADER, i, 256, &len, name);

			logger.info("Suroutine Uniform: " + to_string(i) + "  name: " + name);
			glGetActiveSubroutineUniformiv(p, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

			int* s = (int*)malloc(sizeof(int) * numCompS);
			glGetActiveSubroutineUniformiv(p, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
			logger.info("Compatible Subroutines:");
			for (int j = 0; j < numCompS; ++j) {

				glGetActiveSubroutineName(p, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
				logger.info("\t" + to_string(s[j]) + " - " + name);
			}
			logger.info("");
			free(s);
		}
	}

	void display() override {
		shader("subroutine")([&] {
		//	glGetProgramStageiv(shader("subroutine").program(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &n);
			//GLuint* indices = new GLuint[n];
			//indices[colorLoc] = greenIndex;
			//indices[subLoc] = cyanIndex;

			subroutineFrag("color", "blue");
			subroutineFrag("sub_color", "yellow");
			//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, n, indices);
			send("sub", false);
			shade(quad);
		});
	}

private:
	GLint colorLoc;
	GLint subLoc;
	GLuint redIndex;
	GLuint greenIndex;
	GLuint blueIndex;
	GLuint yellowIndex;
	GLuint cyanIndex;
	GLsizei n;
	ProvidedMesh quad;
	Logger logger = Logger::get("subroutine");
};