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

constexpr uint CONSTS = 0;
constexpr uint COUNTS = 0;
constexpr uint NEXT_ID = 0;
constexpr uint NUM_DATA_ELEMENTS = 4;
constexpr uint DATA = 1;
constexpr uint KEY_IN = 0;
constexpr uint KEY_OUT = 1;
constexpr uint VALUE_IN = 2;
constexpr uint VALUE_OUT = 3;
constexpr uint SUMS = 5;
constexpr uint RADIX_SUM_DATA = 6;
constexpr uint Num_Threads_Per_Block = 1024;
constexpr uint Radix = 256;
constexpr uint Num_Elements = 1 << 20;
constexpr uint R = 32;
constexpr uint L = 8;
constexpr uint Num_Blocks = 64;
constexpr uint Num_Groups_Per_Block = Num_Threads_Per_Block / R;
constexpr uint Num_Elements_Per_Block = nearestMultiple(Num_Elements / Num_Blocks, Num_Threads_Per_Block);
constexpr uint Num_Elements_Per_Group = Num_Elements_Per_Block / Num_Groups_Per_Block;
constexpr uint Num_Radices_Per_WorkGroup = Radix / Num_Blocks;
constexpr uint Num_Groups = Num_Blocks * Num_Groups_Per_Block;

struct Consts {
	uint byte;
	uint R;
	uint Radix;
	uint Num_Groups_per_WorkGroup;
	uint Num_Elements_per_WorkGroup;
	uint Num_Elements_Per_Group;
	uint Num_Elements;
	uint Num_Radices_Per_WorkGroup;
	uint Num_Groups;
};
struct RadixSumData {
	uint mutex = 1;
	uint running_sum = 0;
};

using ConstUniform = UniformBuffer<Consts>;
using RadixSumDataBuffer = StorageBuffer<RadixSumData>;
using NextId = AtomicCounterBuffer;
using RadixSumBuffer = StorageBuffer<uint>;

using DataElements = std::array<uIntBuffer, NUM_DATA_ELEMENTS>;

int main(int argc, const char** argv) {


	auto rng = [] {
		static std::default_random_engine engine{ 123456789 };
		static std::uniform_int_distribution<int> dist(0, 255);
		return dist(engine);
	};

	withGL({ 4, 6 }, [&] {

		constexpr uint Size = Num_Elements;
		std::vector<uint> elements(Size);
		//	std::fill_n(begin(elements), Size, 5);
		std::generate(begin(elements), end(elements), [&rng] { return rng(); });

		std::array<int, Radix> bits;
		std::iota(begin(bits), end(bits), 0);

		std::vector<uint> counts(Radix * Num_Blocks);
		std::vector<std::vector<uint>> counts_per_block;
		for (int i = 0; i < Num_Blocks; i++) {
			int offset = Num_Elements_Per_Block * i;
			int end = offset + Num_Elements_Per_Block;

			for (int j = offset; j < end; j++) {
				if (j < Num_Elements) {
					int bit = elements[j];
					int bitIdx = i * Radix + bit;
					counts[bitIdx]++;
				}
			}
		}

		fmt::print("num elements: {}\n", Size);
		fmt::print("Radix: {}\n", Radix);
		fmt::print("Num Groups Per WorkGroup: {}\n", Num_Groups_Per_Block);
		fmt::print("Num Elements Per Groups: {}\n", Num_Elements_Per_Group);
		fmt::print("{}\n", Num_Elements_Per_Group + R - (Num_Elements_Per_Group % R));

		auto num = nearestMultiple(1 << 16, 192);
		if (num % 12 == 0) printf("%d is mutiple of %d\n", num, 12);
		if (num % 16 == 0) printf("%d is mutiple of %d\n\n", num, 16);


		uIntBuffer countBuffer;
		countBuffer.allocate(Radix * Num_Blocks * Num_Groups_Per_Block, 0);
		countBuffer.fill(0);

		DataElements dElements;
		for (auto i = 0; i < NUM_DATA_ELEMENTS; i++) {
			dElements[i].allocate(Size);
		}

		dElements[KEY_IN].update(&elements[0]); // TODO num elements instead of byte size

		dElements[KEY_IN].read([&](auto ptr) {
			for (int i = 0; i < Size; i++) {
				assert(*(ptr + i) == elements[i]);
			}
			});

		Consts consts{
			0
			, R
			, Radix
			, Num_Groups_Per_Block
			, Num_Elements_Per_Block
			, Num_Elements_Per_Group
			, Num_Elements
			, Num_Radices_Per_WorkGroup
			, Num_Groups };


		ConstUniform uConsts{ consts };

		static auto countRadicesSrc = getText("shader//count_radices.comp");
		Shader countRadices;
		countRadices.load({ GL_COMPUTE_SHADER, countRadicesSrc, "count_radices.comp" });
		countRadices.createAndLinkProgram();

		static auto prefixSumSrc = getText("shader//prefix_sum.comp");
		Shader prefixSum;
		prefixSum.load({ GL_COMPUTE_SHADER, prefixSumSrc, "prefix_sum.comp" });
		prefixSum.createAndLinkProgram();

		static auto reorderSrc = getText("shader//reorder.comp");
		Shader reorder;
		reorder.load({ GL_COMPUTE_SHADER, reorderSrc, "reorder.comp" });
		reorder.createAndLinkProgram();

		RadixSumData radixSumData;
		RadixSumDataBuffer radixSumDataBuffer;
		radixSumDataBuffer.allocate(1);

		RadixSumBuffer radixSumBuffer;
		radixSumBuffer.allocate(Radix + 1);

		NextId nextId;
		nextId.allocate(1);

		fmt::print("\n");
		dElements[KEY_IN].read([](auto ptr) {
			for (int i = 0; i < 10; i++) fmt::print("{} ", *(ptr + i));
			});
		fmt::print("\n");
		dElements[KEY_OUT].read([](auto ptr) {
			for (int i = 0; i < 10; i++) fmt::print("{} ", *(ptr + i));
			});

		countRadices([&] {
			uConsts.bind(CONSTS);
			countBuffer.bind(COUNTS);
			dElements[KEY_IN].bind(DATA + KEY_IN);
			dElements[KEY_OUT].bind(DATA + KEY_OUT);
			dElements[VALUE_IN].bind(DATA + VALUE_IN);
			dElements[VALUE_OUT].bind(DATA + VALUE_OUT);

			glDispatchCompute(Num_Blocks, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		});

		fmt::print("\n");
		dElements[KEY_IN].read([](auto ptr) {
			for (int i = 0; i < 10; i++) fmt::print("{} ", *(ptr + i));
		});
		fmt::print("\n");
		dElements[KEY_OUT].read([](auto ptr) {
			for (int i = 0; i < 10; i++) fmt::print("{} ", *(ptr + i));
		});

		uint sum = 0;
		//countBuffer.read([&](auto ptr) {


		//	int offset = 0;
		//	int end = countBuffer.count();
		//	constexpr int rowLength = Num_Groups_Per_Block * Num_Blocks;

		//	
		//	for (int i = offset; i < end; i++) {

		//			sum += *(ptr + i);
		//			fmt::print("{} ", *(ptr + i));
		//			if ((i + 1) % rowLength == 0) {
		//				auto radix = i / rowLength;
		//				int expected = std::count(begin(elements), std::end(elements), radix);
		//				assert(sum == expected);
		//				fmt::print("\n");
		//				sum = 0;
		//			}
		//			
		//	}
		//});
		//uint sum0 = std::accumulate(begin(counts), end(counts), 0);
		//fmt::print("\nsum: {}\n", sum);
		//fmt::print("sum: {}\n", sum0);



		prefixSum([&] {
			nextId.set(0);
			radixSumDataBuffer.set({});
			radixSumBuffer.fill(0);

			nextId.bind(NEXT_ID);
			uConsts.bind(CONSTS);
			countBuffer.bind(COUNTS);
			radixSumBuffer.bind(SUMS);
			radixSumDataBuffer.bind(RADIX_SUM_DATA);

			glDispatchCompute(Num_Blocks, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			});

	//	std::vector<uint> expectedSums(Radix);
	//	for (auto i = 0; i < Radix; i++) {
	//		expectedSums[i] = std::count(begin(elements), end(elements), i);
	//	}
	////	std::inclusive_scan(begin(expectedSums), end(expectedSums), begin(expectedSums));
	//	radixSumBuffer.read([&expectedSums](auto ptr) {
	//		//std::exclusive_scan(ptr, ptr + Radix, ptr, 0);
	//		for (auto i = 0; i < Radix; i++) {
	//			auto sum = *(ptr + i);
	//			auto expected = expectedSums[i];
	//		//	fmt::print("{}: {}, {}\n", i, sum, expected);
	//			assert(sum == expected);
	//		}
	//		});

		reorder([&] {
			uConsts.bind(CONSTS);
			countBuffer.bind(COUNTS);
			dElements[KEY_IN].bind(DATA + KEY_IN);
			dElements[KEY_OUT].bind(DATA + KEY_OUT);
			dElements[VALUE_IN].bind(DATA + VALUE_IN);
			dElements[VALUE_OUT].bind(DATA + VALUE_OUT);
			radixSumBuffer.bind(SUMS);

			glDispatchCompute(Num_Blocks, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			});

		fmt::print("\n");
		dElements[KEY_OUT].read([&](auto ptr) {
			assert(std::is_sorted(ptr, ptr + Size));

				for (auto i = 0; i < Radix; i++) {
					auto expected = std::count(begin(elements), end(elements), i);
					auto actual = std::count(ptr, ptr + Size, i);
					assert(actual == expected);
				}
		});


	});

	return 0;
}