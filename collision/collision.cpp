#pragma comment(lib, "fmtd.lib")


#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <memory>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include "../GLUtil/include/ncl/gl/Shader.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/util.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include "../GLUtil/include/ncl/gl/buffer_iterator.h"

#include "algorithm.h"

using namespace ncl;
using namespace gl;
using uint = unsigned int;
using IntBuffer = StorageBuffer<int>;
using uIntBuffer = StorageBuffer<uint>;


void test_scan() {
	auto rng = [] {
		static std::default_random_engine engine{ 123456789 };
		static std::uniform_int_distribution<int> dist(0, 20);
		return dist(engine);
	};

	constexpr int size = 1024 * 1024;
	IntBuffer data;
	data.allocate(size);

	//	int content[size] = { 2, 1, 9, 6, 4, , 2, 1,   , 6, , 4, 4, 8, 8, ,     9, 3, 1, , 9, 9, 1,  };
	std::vector<int> content(size);
	std::generate(std::begin(content), std::end(content), [&] { return rng();  });
	data.update(&content[0]);
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
}

constexpr uint nearestMultiple(uint n, uint x) {
	uint nModx = n % x;
	return nModx == 0 ? n : n + x - nModx;
}

constexpr uint Radix = 256;
//constexpr uint Num_Elements = 1 << 20;
constexpr uint Num_Elements = 1 << 16;
//constexpr uint Num_Elements = 65664;
constexpr uint R = 16;
constexpr uint L = 8;
constexpr uint Num_Threads_Per_Block = 192;
constexpr uint Num_Blocks = 16;
constexpr uint Num_Groups_Per_Block = Num_Threads_Per_Block / R;
constexpr uint Num_Elements_Per_Block = Num_Elements / Num_Blocks;
constexpr uint Num_Elements_Per_Group = nearestMultiple(Num_Elements_Per_Block / Num_Groups_Per_Block, R);

struct Consts {
	uint byte;
	uint R;
	uint Radix;
	uint Num_Groups_per_WorkGroup;
	uint Num_Elements_per_WorkGroup;
	uint Num_Elements_Per_Group;
};

using ConstUniform = UniformBuffer<Consts>;

int main(int argc, const char** argv) {

	
	auto rng = [] {
		static std::default_random_engine engine{ 123456789 };
		static std::uniform_int_distribution<int> dist(0, 255);
		return dist(engine);
	};

	withGL({ 4, 6 }, [&] {

		//constexpr uint size = 1 << 16;
		uint Size = Num_Elements;
		std::vector<uint> elements(Size);
		//std::fill_n(begin(elements), Size, 5);
		std::generate(begin(elements), end(elements), [&rng] { return rng(); });

		std::array<int, Radix> bits;
		std::iota(begin(bits), end(bits), 0);

		std::vector<uint> counts(Radix * Num_Blocks);
		std::vector<std::vector<uint>> counts_per_block;
		for (int i = 0; i < Num_Blocks; i++) {
			int offset = Num_Elements_Per_Block * i;
			int end = offset + Num_Elements_Per_Block;
			
			for (int j = offset; j < end; j++) {
				int bit = elements[j];
				int bitIdx = i * Radix + bit;
				counts[bitIdx]++;
			}
		}

		fmt::print("num elements: {}\n", Size);
		fmt::print("Radix: {}\n", Radix);
		fmt::print("Num Groups Per WorkGroup: {}\n", Num_Groups_Per_Block);
		fmt::print("Num Elements Per Groups: {}\n", Num_Elements_Per_Group);
		fmt::print("{}\n", Num_Elements_Per_Group + R - (Num_Elements_Per_Group % R));
		fmt::print("{}\n", counts);

		uIntBuffer countBuffer;
		countBuffer.allocate(Radix * Num_Blocks, 0);
		countBuffer.fill(0);

		uIntBuffer elementBuffer;
		elementBuffer.allocate(Size, 1);
		elementBuffer.update(&elements[0]);

		elementBuffer.read([&](auto ptr) {
			for (int i = 0; i < elementBuffer.count(); i++) {
				assert(*(ptr + i) == elements[i]);
			}
		});

		Consts consts{ 0, R, Radix, Num_Groups_Per_Block, Num_Elements_Per_Block, Num_Elements_Per_Group };
		ConstUniform uConsts{ consts};

		static auto countRadicesSrc = getText("shader//count_radices.comp");
		Shader countRadices;
		countRadices.load({ GL_COMPUTE_SHADER, countRadicesSrc, "count_radices.comp" });
		countRadices.createAndLinkProgram();


		countRadices([&] {
			uConsts.bind(0);
			countBuffer.bind(0);
			elementBuffer.bind(1);

			glDispatchCompute(1, 1, 1);
		});

		uint sum = 0;
		//countBuffer.read([&](auto ptr) {
		//	for (int i = 0; i < (Radix * Num_Blocks); i++) {
		//		if (*(ptr + i) != counts[i]) {
		//			fmt::print("{} != {} at index: {}", *(ptr + i), counts[i], i);
		//			exit(i);
		//		}
		//		fmt::print("{} ", *(ptr + i));
		//		sum += *(ptr + i);
		//	}
		//});
		uint sum0 = std::accumulate(begin(counts), end(counts), 0);
		fmt::print("\nsum: {}\n", sum);
		fmt::print("sum: {}\n", sum0);

	});
	
	return 0;
}