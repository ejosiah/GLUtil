#pragma once

#include <ratio>

namespace ncl {
	namespace unit {

		inline long double operator ""_mm(long double v) {
			return v * 0.1;
		}

		inline long double operator ""_milimeters(long double v) {
			return v * 0.1;
		}

		inline long double operator  ""_cm(long double v) {
			return v;
		}

		inline long double operator  ""_centimeters(long double v) {
			return v;
		}

		inline long double operator ""_m(long double v) {
			return v * 100;
		}

		inline long double operator ""_meters(long double v) {
			return v * 100;
		}

		inline long double operator ""_km(long double v) {
			return v * 100000;
		}

		inline long double operator ""_kilometers(long double v) {
			return v * 100000;
		}

		inline long double operator ""_in(long double v) {
			return v * 2.54;
		}

		inline long double operator ""_inches(long double v) {
			return v * 2.54;
		}

		inline long double operator ""_ft(long double v) {
			return v * 30.48;
		}

		inline long double operator ""_feet(long double v) {
			return v * 30.48;
		}

		inline long double operator ""_mi(long double v) {
			return v * 160934;
		}

		inline long double operator ""_miles(long double v) {
			return v * 160934;
		}

		inline long double operator ""_deg(long double v) {
			return v;
		}
	}
}