#pragma once

#include <thread>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <type_traits>
#include <functional>

namespace ncl {
	namespace concurrency {
		class ThreadPool {
		public:
			using UniqueFunction = std::packaged_task<void()>;

			ThreadPool(unsigned int size);

			~ThreadPool();

			template<class F>
			auto async(F&& func) const;

		protected:
			void enqueue(UniqueFunction task) const;
			void worker_loop();

		private:
			mutable struct {
				std::mutex mtx;
				std::queue<UniqueFunction> work_queue;
				bool aborting = false;
			} m_state;
			std::vector<std::thread> m_workers;
			mutable std::condition_variable m_cv;
		};

		ThreadPool::ThreadPool(unsigned int size) {
			for (unsigned int i = 0; i < size; i++) {
				m_workers.emplace_back([this]() { worker_loop(); });
			}
		}

		ThreadPool::~ThreadPool() {
			if (std::lock_guard lk(m_state.mtx); true) {
				m_state.aborting = true;
			}
			m_cv.notify_all();
			for (std::thread& t : m_workers) {
				t.join();
			}
		}

		template<class F>
		auto ThreadPool::async(F&& func)  const {
			using ResultType = std::invoke_result_t<std::decay_t<F>>;
			std::packaged_task<ResultType()> pt(std::forward<F>(func));
			std::future<ResultType> future = pt.get_future();

			auto capture = [](auto& p) {
				using T = std::decay_t<decltype(p)>;
				return std::make_shared<T>(std::move(p));
			};

			UniqueFunction task( [pt = capture(pt)] () mutable {
				pt->operator()();
			} );

			enqueue(std::move(task));

			return future;
		}

		void ThreadPool::enqueue(ThreadPool::UniqueFunction task)  const {
			if (std::lock_guard lk(m_state.mtx); true) {
				m_state.work_queue.push(std::move(task));
			}
			m_cv.notify_one();
		}

		void ThreadPool::worker_loop() {
			while (true) {
				std::unique_lock lk(m_state.mtx);
				while (m_state.work_queue.empty() && !m_state.aborting) {
					m_cv.wait(lk);
				}
				if (m_state.aborting) break;
			//	assert(!m_state.work_queue.empty() == true);
				UniqueFunction task = std::move(m_state.work_queue.front());
				m_state.work_queue.pop();

				lk.unlock();
				task();
			}
		}
	}
}