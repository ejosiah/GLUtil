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


		constexpr size_t DATA_PER_WORKGROUP = 1024;

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


		template<typename BufferSource>
		void sort(BufferSource& bufferSource, std::optional<BufferSource>& indices) {
			GLuint buffer;
			if constexpr (std::is_integral<BufferSource>::value)
				buffer = bufferSource;

			else
				buffer = bufferSource.buffer();
		}
	}
}