#pragma once

namespace ncl {

	template<typename T>
	class numeric_iterator {
	public:
		numeric_iterator(T t = T())
			:t{ t }
		{}

		T operator*() const {
			return t;
		}

		auto& operator++() {
			t++;
			return *this;
		}

		auto operator++(int) {
			auto snapshot = *this;
			++* this;
			return snapshot;
		}

		auto& operator--() {
			t--;
			return *this;
		}

		auto operator--(int) {
			auto snapshot = *this;
			--* this;
			return snapshot;
		}

	private:
		T t;
	};

	using int_iterator = numeric_iterator<int>;
	using uint_iterator = numeric_iterator<unsigned>;
}