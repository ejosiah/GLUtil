#pragma once

#include <gl/gl_core_4_5.h>
#include <functional>
#include "../type_wrapper.h"

namespace ncl {
	namespace gl {
		template<typename T>
		class StorageBufferObj {
		public:
			StorageBufferObj() = default;

			explicit StorageBufferObj(T, GLuint = 0);

			explicit StorageBufferObj(size_t count, GLuint id = 0);

			explicit StorageBufferObj(std::vector<T>, GLuint = 0);

			explicit StorageBufferObj(Shape&, GLuint = 0);

			StorageBufferObj(StorageBufferObj<T>&&) noexcept;

			StorageBufferObj(const StorageBufferObj<T>&) = delete;

			~StorageBufferObj();

			StorageBufferObj<T>& operator=(const StorageBufferObj<T>&) = delete;

			StorageBufferObj<T>& operator=(StorageBufferObj<T>&&);

			T* get();

			T getFromGPU();

			T getFromGPU(unsigned int index);

			inline GLuint buffer() {
				return _buf;
			}

			inline size_t count() {
				return _size / sizeof(T);
			}

			template<typename U>
			friend void send(const StorageBufferObj<U>&);

			template<typename U>
			friend void transfer(StorageBufferObj<U>&, StorageBufferObj<U>&);

			void sendToGPU(bool update = true);

			static inline GLsizeiptr sizeOf(std::vector<T> v) {
				return sizeof(T) * v.size();
			}

			static inline GLsizeiptr sizeOf(int count) {
				return sizeof(T) * count;
			}
			
			void read(std::function<void(T*)> use);

		private:
			std::vector<T> _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;

		}; 
		
	}

}
#include "detail/StorageBufferObj.inl"