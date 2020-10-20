#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include "algorithm.h"

int main(int argc, const char** argv) {

	using namespace ncl::gl;
	using IntBuffer = StorageBuffer<int>;
	
	auto rng = [] {
		static std::default_random_engine engine{ 123456789 };
		static std::uniform_int_distribution<int> dist(0, 20);
		return dist(engine);
	};

	withGL({ 4, 6 }, [&] {
		constexpr int size = 10240;
		IntBuffer data;
		data.allocate(size);

	//	int content[size] = { 2, 1, 9, 6, 4, , 2, 1,   , 6, , 4, 4, 8, 8, ,     9, 3, 1, , 9, 9, 1,  };
		int content[size];
		std::generate(std::begin(content), std::end(content), [&] { return rng();  });
		data.update(content);
		int l = 1023;
		int u = 1048;
//		for(auto& val : content) { std::cout << val << " "; };
		//for (int i = l; i < u; i++) std::cout << content[i] << " ";
		std::exclusive_scan(std::begin(content), std::end(content), std::begin(content), 0);
		std::cout << "\n";
		for (int i = l; i < u; i++) std::cout << content[i] << " ";
		std::cout << "\n";

		scan(data);
		data.read([&](auto itr) {
			for (int i = l; i < u; i++)
				std::cout << *(itr + i) << " ";

			printf("\n");
			for (int i = 0; i < size; i++) {
				if (*(itr + i) != content[i]) {
					printf("%d != %d at index %d\n", *(itr + i), content[i], i);
					assert(*(itr + i) == content[i]);
				//	exit(i);
				}
			}
		});



	});
	
	return 0;
}