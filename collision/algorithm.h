#pragma once

#include <type_traits>
#include <algorithm>
#include <optional>
#include "../GLUtil/include/ncl/gl/Shader.h"
#include "../GLUtil/include/ncl/gl/shader_binding.h"
#include "../GLUtil/include/ncl/gl/util.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"


namespace ncl {
	namespace gl {

		using uint = unsigned int;
		using IntBuffer = StorageBuffer<int>;
		using uIntBuffer = StorageBuffer<uint>;

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
		constexpr uint R = 32;
		constexpr uint L = 8;
		



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

		constexpr int nearestPowerOfTwo(int x) {
			if (x <= 1) return 2;
			x -= 1;
			x |= (x >> 1);
			x |= (x >> 2);
			x |= (x >> 4);
			x |= (x >> 8);
			x |= (x >> 16);
			x += 1;
			return x;
		}

		constexpr uint nearestMultiple(uint n, uint x) {
			uint nModx = n % x;
			return nModx == 0 ? n : n + x - nModx;
		}


		constexpr size_t DATA_PER_WORKGROUP = 1024;

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

		template<typename BufferSource>
		void scan(BufferSource& bufferSource) {
			static auto prefix_scan_source = getText("shader//prefix_scan.comp");
			static auto add_value_source = getText("shader//add_value.comp");

			Shader scan_program;
			Shader add_value_program;

			scan_program.load({ GL_COMPUTE_SHADER, prefix_scan_source, "prefix_scan.comp" });
			scan_program.createAndLinkProgram();

			add_value_program.load({ GL_COMPUTE_SHADER, add_value_source, "add_value.comp" });
			add_value_program.createAndLinkProgram();

			GLuint buffer;
			 if constexpr (std::is_integral<BufferSource>::value)
				buffer = bufferSource;
			
			else 
				buffer = bufferSource.buffer();
			
			 StorageBuffer<GLuint> source_buffer;
			 source_buffer.reference(buffer);

			 StorageBuffer<GLuint> data_buffer;
			 auto size = std::max(DATA_PER_WORKGROUP, source_buffer.count());
			 auto remainder = size % DATA_PER_WORKGROUP;
			 size += remainder != 0 ? (DATA_PER_WORKGROUP - remainder) : 0;
			 data_buffer.allocate(size);
			 data_buffer.copy(source_buffer.buffer());


			 StorageBuffer<GLuint> sum_buffer;

			 int count = data_buffer.count();
			 GLuint numWorkGroups = std::max(1, int(count / DATA_PER_WORKGROUP));
			 sum_buffer.allocate(numWorkGroups);

			 
			 scan_program([&] {
				 data_buffer.bind(0);
				 sum_buffer.bind(1);
				 send("N", int(count));
				 glDispatchCompute(numWorkGroups, 1, 1);
				 glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			});

			 
			 scan_program([&] {
				 // TODO will break if sum_buffer.count() > 1024
				 // generate sums in previous scan block
				 sum_buffer.bind(0);
				 send("N", int(sum_buffer.count()));
				 glDispatchCompute(numWorkGroups/ sum_buffer.count(), 1, 1);
				 glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			});

			 sum_buffer.read([n = numWorkGroups](auto ptr) {
				 for (auto i = 0; i < 50; i++) {
					 printf("%d ", *(ptr + i));
				 }
				 });
			 printf("\n");



			 add_value_program([&] {
				 sum_buffer.bind(0);
				 data_buffer.bind(1);
				 send("count", int(sum_buffer.count()));
				 glDispatchCompute(numWorkGroups, 1, 1);
				 glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			 });


			 source_buffer.copy(data_buffer.buffer());
		}

		enum QUERY { HISTOGRAM, PREFIX_SUM, REORDER, NUM_QUERIES };
		GLuint queires[NUM_QUERIES];
		std::vector<float> stats[NUM_QUERIES];

		template<typename BufferSource>
		void sort(BufferSource& bufferSource) {
			static Shader countRadices({ GL_COMPUTE_SHADER, getText("shader//count_radices.comp"), "count_radices.comp" });
			static Shader prefixSum({ GL_COMPUTE_SHADER, getText("shader//prefix_sum.comp"), "prefix_sum.comp" });
			static Shader reorder({ GL_COMPUTE_SHADER, getText("shader//reorder.comp"), "reorder.comp" });

			glGenQueries(NUM_QUERIES, queires);

			GLuint buffer;
			if constexpr (std::is_integral<BufferSource>::value)
				buffer = bufferSource;

			else
				buffer = bufferSource.buffer();

			DataElements elements;
			elements[KEY_IN].reference(buffer);

			uint Num_Elements = elements[KEY_IN].count();
			uint Num_Blocks = 64;
			uint Num_Groups_Per_Block = Num_Threads_Per_Block / R;
			uint Num_Elements_Per_Block = nearestMultiple(Num_Elements / Num_Blocks, Num_Threads_Per_Block);
			uint Num_Elements_Per_Group = Num_Elements_Per_Block / Num_Groups_Per_Block;
			uint Num_Radices_Per_WorkGroup = Radix / Num_Blocks;
			uint Num_Groups = Num_Blocks * Num_Groups_Per_Block;

			for (auto i = 1; i < NUM_DATA_ELEMENTS; i++) {
				elements[i].allocate(Num_Elements);
			}

			uIntBuffer countBuffer;
			countBuffer.allocate(Radix * Num_Blocks * Num_Groups_Per_Block, 0);
			countBuffer.fill(0);

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
					elements[KEY_IN].bind(DATA + KEY_IN);
					elements[KEY_OUT].bind(DATA + KEY_OUT);
					elements[VALUE_IN].bind(DATA + VALUE_IN);
					elements[VALUE_OUT].bind(DATA + VALUE_OUT);

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
					elements[KEY_IN].bind(DATA + KEY_IN);
					elements[KEY_OUT].bind(DATA + KEY_OUT);
					elements[VALUE_IN].bind(DATA + VALUE_IN);
					elements[VALUE_OUT].bind(DATA + VALUE_OUT);
					radixSumBuffer.bind(SUMS);

					query<GL_TIME_ELAPSED>(queires[REORDER], [&] {
						glDispatchCompute(Num_Blocks, 1, 1);
					});

					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
					});

				std::swap(elements[KEY_IN], elements[KEY_OUT]);
				std::swap(elements[VALUE_IN], elements[VALUE_OUT]);


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
		}
	}
}