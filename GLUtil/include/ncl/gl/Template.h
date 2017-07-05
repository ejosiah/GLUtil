#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <cstdarg>
#include "util.h"

namespace ncl {
	class Template {
	public:
		Template(std::string t) :text(t) {
		}

		std::string execute(...) {
			const char* format = text.c_str();
			int size = text.size() + 20;
			char* out = new char[256];
			va_list args;
			va_start(args, format);
			sprintf_s(out, 256, format, args);
			va_end(args);

			std::string res;
			for (char* c = out; c != '\0'; c++) {
				res.push_back(*c);
			}
			delete[] out;
			return res;
		}

	private:
		std::string text;
	};
}