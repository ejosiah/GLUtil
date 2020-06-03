
#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "LightProbScene.h"
#include "../GLUtil/include/ncl/concurrency/concurrency.h"

//void test() {
//	std::atomic<int> sum(0);
//	ncl::concurrency::ThreadPool tp(4);
//	std::vector<std::future<int>> futures;
//	for (int i = 0; i < 60000; ++i) {
//		auto f = tp.async([i, &sum]() {
//			sum += i;
//			return i;
//			});
//		futures.push_back(std::move(f));
//	}
//	assert(futures[42].get() == 42);
//	assert(903 <= sum && sum <= 1799970000);
//}

int main() {
	auto scene = new LightProbeScene;
	start(scene);

	//vector<future<int>> futures;
	//for (int i = 0; i < 10; i++) {
	//	auto f = async([=] { return i; });
	//	futures.push_back(std::move(f));
	//}

	//auto f = ncl::concurrency::sequence(futures);
	//auto res = f.get();
	//for (auto i : res) cout << i << " ";
	//cout << endl << thread::hardware_concurrency() << endl;



	//cin.get();
	
	return 0;
}