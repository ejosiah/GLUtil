#pragma once

#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {

		template<typename U>
		struct ObjectReflect {

			template<typename U>
			static GLsizeiptr sizeOfObj(U& obj);

			template<typename U>
			static void* objPtr(U& obj);

			template<typename U>
			static GLsizeiptr sizeOf(int size);
		};
		
		template<typename T>
		class StorageBufferObj {
		public:
			StorageBufferObj() = default;

			explicit StorageBufferObj(T, GLuint = 0);

			explicit StorageBufferObj(int count, GLuint id = 0);

			StorageBufferObj(StorageBufferObj<T>&&) noexcept;

			StorageBufferObj(const StorageBufferObj<T>&) = delete;

			~StorageBufferObj();

			StorageBufferObj<T>& operator=(const StorageBufferObj<T>&) = delete;

			StorageBufferObj<T>& operator=(StorageBufferObj<T>&&);

			T& get();

			T getFromGPU();

			inline GLuint buffer() {
				return _buf;
			}

			template<typename U>
			friend void send(const StorageBufferObj<U>&);

			template<typename U>
			friend void transfer(StorageBufferObj<U>&, StorageBufferObj<U>&);

			void sendToGPU(bool update = true);

		private:
			T _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;

		}; 
		
	}

}
#include "detail/StorageBufferObj.inl"