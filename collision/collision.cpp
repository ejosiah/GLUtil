#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>

int main(int argc, const char** argv) {

	using namespace ncl::gl;
	using IntBuffer = StorageBuffer<int>;
	withGL({ 4, 6 }, [] {
		IntBuffer data;
		data.allocate(8);

		int content[]{ 3, 1, 7, 0, 4, 1, 6, 3 };
		data.update(content);

		auto buffer = GlBuffer<int>{ 0, data.buffer() };
		auto begin = buffer.begin();
		auto end = buffer.end();
		std::for_each(begin, end, [](auto val) { std::cout << val << " "; });

		std::exclusive_scan(begin, end, begin, 0);
		std::cout << "\n";
		std::for_each(begin, end, [](auto val) { std::cout << val << " "; });
		std::cout << "\n";

	});
	
	return 0;
}