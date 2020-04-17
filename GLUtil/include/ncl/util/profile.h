#pragma once
#include <chrono>
#include <functional>
#include <string>
#include <cmath>

namespace ncl {

	using clock = std::chrono::steady_clock;
	using func = std::function<void()>;

	inline clock::duration profile(func func) {
		using namespace std::chrono;
		auto start = clock::now();
		func();
		return clock::now() - start;
	}

	inline std::string print(clock::duration duration) {
		double d = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * 1.0;
		if (d < 1000) return std::to_string(d) + " milliseconds";

		d = d / 1000;
		auto s = floor(d);
		auto ms = d - s;
		if (s < 60) return std::to_string(s) + " seconds, " + std::to_string(ms) + " milliseconds";
		d = s / 60;
		auto m = floor(d);
		s = d - m;
		return std::to_string(m) + " minutes, " + std::to_string(s) + "seconds, " + std::to_string(ms) + " milliseconds";
	}
}