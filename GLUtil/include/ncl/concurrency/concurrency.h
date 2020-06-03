#pragma once

#include <vector>
#include <future>
#include <thread>
#include "thread_pool.h"

namespace ncl {
	namespace concurrency {

		const ThreadPool globalThreadPool{ std::thread::hardware_concurrency() };

		template<typename T>
		inline std::future<std::vector<T>> sequence(std::vector<std::future<T>>& futures) {
			return globalThreadPool.async([&futures] {
				std::vector<T> result;
				for (auto& f : futures) {
					result.push_back(f.get());
				}
				return result;
			});

		}

	}
}