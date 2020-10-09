#pragma once

#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/gl/BufferObject.h"
#include "../GLUtil/include/ncl/gl/CopyBuffer.h"
#include <algorithm>

namespace ncl {
	namespace gl {

		constexpr size_t Radix = 256;
		constexpr int IntSize = 4;

		class RadixSort : public Compute, public CopyBuffer {
		public:
			RadixSort(TextureBuffer& input, Shader* shader)
				:Compute(glm::vec3(1), {}, shader)
				, _input{ input }
			{
			//	_count = StorageBuffer<unsigned int>{ Radix, 0 };
				//_count.read([](auto ptr) {
				//	for (int i = 0; i < Radix; i++) {
				//		*(ptr+i) = 0;
				//	}
				//});
				std::vector<unsigned int> data(Radix);
				_counter = TextureBuffer{"count", &data[0], sizeof(unsigned int) * Radix, GL_R32UI, 0, 0, GL_DYNAMIC_READ};
				_counter_locks = TextureBuffer{"count_locks", &data[0], sizeof(unsigned int) * Radix, GL_R32UI, 0, 0, GL_DYNAMIC_READ};

				auto [size, auxId] = copy(input.buffer());
				_aux = TextureBuffer{ "aux", nullptr, size, GL_R32UI, auxId };
				_index_buffer = TextureBuffer{ "index_buffer", nullptr, input.size() * 2, GL_R32UI};
			}

			void preCompute() override {
				glBindImageTexture(0, _counter.tbo_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(1, _counter_locks.tbo_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(2, _input.tbo_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(3, _aux.tbo_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(4, _index_buffer.tbo_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				auto numItems = _input.size(IntSize);
				_shader->sendUniform1i("numItems", numItems);
			}

			void postCompute() override {
				static std::stringstream sbr;
				sbr.clear();
				sbr.str("\n");
				glBindBuffer(GL_TEXTURE_BUFFER, _counter_locks.buffer());
				auto ptr = (unsigned int*)glMapNamedBuffer(_counter_locks.buffer(), GL_READ_WRITE);
				for (int i = 0; i < 256; i++) {
					auto val = *(ptr+i);
					sbr << val << "\n";
				//	if (val != 0) {
						
				//	}
				}
				logger.info(sbr.str());
				glUnmapNamedBuffer(_counter_locks.buffer());
				glBindBuffer(GL_TEXTURE_BUFFER, 0);

				//auto size = _index_buffer.size(IntSize);
				//glBindBuffer(GL_TEXTURE_BUFFER, _index_buffer.buffer());
				//auto ptr = (unsigned int*)glMapNamedBuffer(_index_buffer.buffer(), GL_READ_WRITE);
				//for (int i = 0; i < size; i++) {
				//	auto val = *(ptr+i);
				////	if (val == 0) {
				//		logger.info(std::to_string(i) + ": " + std::to_string(val));
				////	}
				//}
				//glUnmapNamedBuffer(_index_buffer.buffer());
				//glBindBuffer(GL_TEXTURE_BUFFER, 0);

			}

		private:
			TextureBuffer& _input;
			TextureBuffer _index_buffer;
			TextureBuffer _aux;
			TextureBuffer _counter;
			TextureBuffer _counter_locks;
			Logger logger = Logger::get("RadixSort");
		};
	}
}