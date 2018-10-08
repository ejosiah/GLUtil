#pragma once

#include <string>
#include <tuple>
#include <ncl/gl/Shader.h>
#include <ncl/gl/TransformFeedBack.h>
#include <ncl/gl/textures.h>
#include <ncl/gl/shader_binding.h>
#include <glm/glm.hpp>
#include <ncl/gl/Font.h>
#include <algorithm>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class Test {
public:
	Test(string msg):msg(msg) {

	}

	Test(string msg, string shaderPath, tuple<void*, GLuint> data, float expected):
		msg(msg), shaderPath(shaderPath), testData(get<0>(data)), size(get<1>(data)), expected(expected){
		testId = nextTestId++;
	}

	virtual void setHeight(float value) {
		height = value;
	}

	virtual void init() {
		passFont = Font::Arial(15, Font::BOLD, GREEN);
		failFont = Font::Arial(15, Font::BOLD, RED);
		shader = new Shader();
		shader->storePreprocessedShaders(true);
		shader->loadFromFile(shaderPath);
		shader->createAndLinkProgram();
		Mesh m;
		m.positions.push_back(glm::vec3(0));
		mesh = new ProvidedMesh(m);
		tfb = new TransformFeebBack("", true, varyings, 2, shader);
		inputBuffer = new TextureBuffer("testData", testData, size, GL_RGBA32F, 0, 1, GL_DYNAMIC_READ);

		glm::vec3 output { 0, 0, 0 };
		glm::vec4 v{ 0 };
		tfbBuffer = new TextureBuffer("test_output", nullptr, sizeof(float) * 3, GL_RGBA32F, 0, 2, GL_DYNAMIC_READ);
		extraData = new TextureBuffer("extra_data", nullptr, sizeof(float) * 4, GL_RGBA32F, 0, 3, GL_DYNAMIC_READ);
	
	}

	virtual void run() {
		if (!done) {
			(*shader)([&] {
				send(inputBuffer);
				GLuint buffer[2] = { tfbBuffer->buffer(), extraData->buffer() };
				mesh->use(0, [&]() {
					(*tfb)(buffer, 2, GL_POINTS, [&]() {
						glDrawArrays(GL_POINTS, 0, 1);
					});
				});
			});
			checkResult();
			done = true;
			afterTest();
		}
		printResult();
	}

	virtual void afterTest() {

	}

	void checkResult() {
		glBindBuffer(GL_ARRAY_BUFFER, tfbBuffer->buffer());
		vec3 data = *(vec3*)glMapNamedBuffer(tfbBuffer->buffer(), GL_READ_ONLY);
		passed = data.x == expected;
		glUnmapNamedBuffer(tfbBuffer->buffer());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	virtual void printResult() {
		if (passed) {
			passFont->render(msg + ": PASSED", 10, height - (40 + testId * 20));
		}
		else {
			failFont->render(msg + ": FAILED", 10, height - (40 + testId * 20));
		}
	}

	friend class TestSuit;

protected:
	float height;
	float expected = 0.0;
	Font * passFont;
	Font* failFont;
	bool passed = false;
	TransformFeebBack* tfb;
	TextureBuffer* tfbBuffer;
	TextureBuffer* extraData;
	TextureBuffer* inputBuffer;
	ProvidedMesh* mesh;
	string msg;
	string shaderPath;
	Shader* shader;
	void* testData;
	GLuint size;
	bool done = false;
	int testId;
	static int nextTestId;
	const char* varyings[2] = { "test_output", "extra_data" };

};

int Test::nextTestId;

class TestSuit : public Test {
public:
	TestSuit(string name, vector<Test*> tests) 
		:Test(name), name(name), tests(tests) {
		testId = tests[0]->testId;
		for (auto test : tests) {
			test->msg = "\t" + test->msg;
			test->testId = test->testId + 1;
		}
	}

	virtual void init() override {
		for (auto test : tests) {
			test->init();
		}
	}

	virtual void run() override {
		for (auto test : tests) {
			test->run();
		}
		//passed = all_of(tests.begin(), tests.end(), [](Test* t) { return t->passed; });
		//if (passed) {
		//	tests[0]->passFont->render(msg + " PASSED:", 10, height - (40 + testId * 20));
		//}
		//else {
		//	tests[0]->failFont->render(msg + " FAILED:", 10, height - (40 + testId * 20));
		//}
		
	}

public:
	string name;
	vector<Test*> tests;
	bool done;
	bool passed;
};