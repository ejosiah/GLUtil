#pragma once

#include <ratio>

namespace ncl {
	namespace unit {

		// length

		inline long double operator ""_mm(long double l) {
			return l * 0.001;
		}
		
		inline long double operator ""_cm(long double l) {
			return l * 0.01;
		}

		inline long double operator ""_m(long double l) {
			return l;
		}

		inline long double operator ""_km(long double l) {
			return l * 1000;
		}

		inline long double operator ""_in(long double l) {
			return l * 0.0254;
		}

		inline long double operator ""_ft(long double l) {
			return l * 0.3048;
		}

		inline long double operator ""_mi(long double l) {
			return l * 1609.344;
		}

		// mass
		inline long double operator ""_mg(long double m) {
			return 0.000001 * m;
		}

		inline long double operator ""_g(long double m) {
			return 0.001 * m;
		}

		inline long double operator ""_kg(long double m) {
			return m;
		}

		inline long double operator ""_lb(long double m) {
			return 0.453592 * m;
		}

		inline long double operator ""_st(long double m) {
			return 6.350293 * m;
		}

	}
}