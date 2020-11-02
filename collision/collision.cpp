#pragma comment(lib, "fmtd.lib")


#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <memory>
#include <limits>
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
		static std::uniform_int_distribution<int> dist(0, 1 << 20);
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

enum QUERY{ HISTOGRAM, PREFIX_SUM, REORDER, NUM_QUERIES};

constexpr bool debug = true;

template<GLenum TARGET, typename Func>
constexpr void query(GLuint id, Func&& func) {
	if constexpr (debug) {
		glBeginQuery(TARGET, id);
	}
	func();

	if constexpr (debug) {
		glEndQuery(TARGET);
	}
}

int main(int argc, const char** argv) {


	auto rng = [] {
		static std::default_random_engine engine{ 123456789 };
		static std::uniform_int_distribution<int> dist(0, 255);
		return dist(engine);
	};


	withGL({ 4, 6 }, [&] {

		GLuint queires[NUM_QUERIES];
		std::vector<float> stats[NUM_QUERIES];

		glGenQueries(NUM_QUERIES, queires);

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

		
		
		for (int i = 0; i < 4; i++) {

			uConsts.update([byte = i](auto consts) { consts->byte = byte; });

			countRadices([&] {
				uConsts.bind(CONSTS);
				countBuffer.bind(COUNTS);
				dElements[KEY_IN].bind(DATA + KEY_IN);
				dElements[KEY_OUT].bind(DATA + KEY_OUT);
				dElements[VALUE_IN].bind(DATA + VALUE_IN);
				dElements[VALUE_OUT].bind(DATA + VALUE_OUT);

				query<GL_TIME_ELAPSED>(queires[HISTOGRAM], [&] {
					glDispatchCompute(Num_Blocks, 1, 1);
				});

				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			});


			prefixSum([&] {
				nextId.set(0);
				radixSumDataBuffer.set({});
				radixSumBuffer.fill(0);

				nextId.bind(NEXT_ID);
				uConsts.bind(CONSTS);
				countBuffer.bind(COUNTS);
				radixSumBuffer.bind(SUMS);
				radixSumDataBuffer.bind(RADIX_SUM_DATA);

				query<GL_TIME_ELAPSED>(queires[PREFIX_SUM], [&] {
					glDispatchCompute(Num_Blocks, 1, 1);
				});
				
				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				});


			reorder([&] {
				uConsts.bind(CONSTS);
				countBuffer.bind(COUNTS);
				dElements[KEY_IN].bind(DATA + KEY_IN);
				dElements[KEY_OUT].bind(DATA + KEY_OUT);
				dElements[VALUE_IN].bind(DATA + VALUE_IN);
				dElements[VALUE_OUT].bind(DATA + VALUE_OUT);
				radixSumBuffer.bind(SUMS);

				glBeginQuery(GL_TIME_ELAPSED, queires[PREFIX_SUM]);
				query<GL_TIME_ELAPSED>(queires[PREFIX_SUM], [&] {
					glDispatchCompute(Num_Blocks, 1, 1);
				});

				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				});

			std::swap(dElements[KEY_IN], dElements[KEY_OUT]);
			std::swap(dElements[VALUE_IN], dElements[VALUE_OUT]);


			if constexpr (debug) {
				GLint duration;
				glGetQueryObjectiv(queires[HISTOGRAM], GL_QUERY_RESULT, &duration);
				stats[HISTOGRAM].push_back(duration * 1e-6f);

				glGetQueryObjectiv(queires[PREFIX_SUM], GL_QUERY_RESULT, &duration);
				stats[PREFIX_SUM].push_back(duration * 1e-6f);

				glGetQueryObjectiv(queires[REORDER], GL_QUERY_RESULT, &duration);
				stats[REORDER].push_back(duration * 1e-6f);
			}
		}


		if constexpr (debug) {
			fmt::print("\n");
			dElements[KEY_OUT].read([&](auto ptr) {
				assert(std::is_sorted(ptr, ptr + Size));

				//for (int i = 0; i < (1 << 16); i++) fmt::print("{} ", *(ptr + i));
				//fmt::print("\n");

				for (auto i = 0; i < Radix; i++) {
					auto expected = std::count(begin(elements), end(elements), i);
					auto actual = std::count(ptr, ptr + Size, i);
					assert(actual == expected);
				}
				});


			float sum = std::accumulate(begin(stats[HISTOGRAM]), end(stats[HISTOGRAM]), 0);
			float avg = sum / 4;
			fmt::print("{:<20}{:<20}{:<20}\n", "Step", "Average (ms)", "Total (ms)");
			fmt::print("{:<20}{:<20}{:<20}\n", "Histogram", avg, sum);

			sum = std::accumulate(begin(stats[PREFIX_SUM]), end(stats[PREFIX_SUM]), 0);
			avg = sum / 4;
			fmt::print("{:<20}{:<20}{:<20}\n", "Prefix Sum", avg, sum);

			sum = std::accumulate(begin(stats[REORDER]), end(stats[REORDER]), 0);
			avg = sum / 4;
			fmt::print("{:<20}{:<20}{:<20}\n","Reorder", avg, sum);
		}

	});

	return 0;
}