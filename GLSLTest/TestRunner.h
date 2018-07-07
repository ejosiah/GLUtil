#pragma once

#include <ncl/gl/Scene.h>
#include "Test.h"

class TestRunner : public Scene {
public:
	TestRunner(vector<Test*> tests) :Scene("Test Runner"), tests(tests) {
		for (auto test : tests) {
			test->setHeight(_height);
		}
	}

	virtual void init() override {
		for (auto test : tests) {
			test->init();
		}
	}

	virtual void display() override {
		for (auto test : tests) {
			test->run();
		}
	}

	virtual void resized() override {

	}

private:
	vector<Test*> tests;
};