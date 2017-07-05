#pragma once

#include <string>
#include <stdexcept>
#include <random>
#include <functional>
#include <cmath>
#include <atomic>

namespace ncl {

	using real = float;

	std::string extractExt(const std::string& path) {
		auto i = path.find_last_of(".");
		if (i == std::string::npos) return "";
		return path.substr(i + 1, path.length());
	};

	std::string getEnv(const char* name) {
		char* value;
		size_t size;
		auto error = _dupenv_s(&value, &size, name);
		if (error) throw std::runtime_error(std::string("no such environment variable: ") + name);
		return value;
	}

	bool isBlankLine(std::string& line) {
		if (line.empty()) return true;
		if (line.size() == 2) {
			size_t found = line.find("\r\n");
			if (found == std::string::npos) return true;
		}
		return false;
	}

	std::string getText(const std::string& filename) {
		std::ifstream fp;
		fp.open(filename.c_str(), std::ios_base::in);
		if (!fp) {
			std::string msg = "unable to open " + filename;
			throw std::runtime_error(msg);
		}
		std::string line, buffer;
		while (getline(fp, line)) {
			if (!line.empty()) {
				buffer.append(line);
				buffer.append("\r\n");
			}
		}
		return buffer;

	}

	unsigned nextSeed() {
		std::random_device rnd;
		return rnd();
	}

	std::function<int(void)> rngInt(int min, int max, unsigned int seed = nextSeed()) {
		using namespace std;
		default_random_engine eng{ seed };
		uniform_int_distribution<int> dist{ min, max };
		return bind(dist, eng);
	}

	std::function<real(void)> rngReal(real min, real max, unsigned int seed = nextSeed()) {
		using namespace std;
		default_random_engine eng{ seed};
		uniform_real_distribution<real> dist{ min, max };
		return bind(dist, eng);
	}

	class Random {
	protected:
		mutable std::function<real(void)> rng;

		void init() {
			std::mt19937 eng{ nextSeed() };
			std::uniform_real_distribution<real> dist{ 0.0, 1.0 };

			rng = std::bind(dist, eng);
		}
	public:
		Random() {
			init();
		}

		real _real(real min, real max) const {
			return rng() * (max - min) + min;
		}

		int _int(int x) const{
			return round((rng() * x));
		}

		glm::vec3 vector(glm::vec3& min, glm::vec3 max) const{
			return glm::vec3{
				_real(min.x, max.x),
				_real(min.y, max.y),
				_real(min.z, max.z)
			};
		}

	};

	class RandomInt {
		std::function<int(void)> generator;

	public:
		RandomInt(int max) : RandomInt(0, max) {}

		RandomInt(int min, int max) {
			generator = rngInt(0, max);
		}

		int operator()() {
			return generator();
		}
	};
}
