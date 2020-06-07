#pragma once

#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {

		//template<typename U>
		//struct ObjectReflect {

		//	template<typename U>
		//	static GLsizeiptr sizeOfObj(U& obj);

		//	template<typename U>
		//	static void* objPtr(U& obj);

		//	template<typename U>
		//	static GLsizeiptr sizeOf(int size);
		//};
		
		template<typename T>
		class StorageBufferObj {
		public:
			StorageBufferObj() = default;

			explicit StorageBufferObj(T*, int count = 1, bool map = false, GLuint = 0);

			explicit StorageBufferObj(int count, bool map = false, GLuint = 0);

			StorageBufferObj(StorageBufferObj<T>&&) noexcept;

			StorageBufferObj(const StorageBufferObj<T>&) = delete;

			~StorageBufferObj();

			StorageBufferObj<T>& operator=(const StorageBufferObj<T>&) = delete;

			StorageBufferObj<T>& operator=(StorageBufferObj<T>&&);

			inline void flush() {
				if (_mapped) {
					glFlushMappedBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeOf<T>(_count));
				}
			}

			T& get();

			T getFromGPU();

			inline GLuint buffer() {
				return _buf;
			}

			template<typename U>
			static inline GLsizeiptr sizeOf(int size) {
				return sizeof(U) * size;
			}

			template<typename U>
			friend void send(const StorageBufferObj<U>&);

			template<typename U>
			friend void transfer(StorageBufferObj<U>&, StorageBufferObj<U>&);

			void sendToGPU(bool update = true);

		private:
			T* _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;
			int _count;
			bool _mapped;

		}; 
		
	}

}
#include "detail/StorageBufferObj.inl"